/****************************************************************************
**
** Copyright (C) 2016 Andy Bray
**
** This file is part of AbZip.
**
** AbZip is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AbZip is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AbZip.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "ZipCentralDir.h"
#include "abzip_p.h"
#include "iobuffer.h"
#include "Utils.h"

#include <QDebug>

ZipCentralDir::ZipCentralDir() : iteratorEntries(sortedEntries)
{
    startOfEndOfCD = -1;
    bytesBeforeZip = 0;
    endOfCentralDir64 = NULL;
    modified = false;
    isValid = true;      // state for a new central directory
}

ZipCentralDir::~ZipCentralDir()
{
    reset();
}

void ZipCentralDir::reset()
{
    qDeleteAll( entries );
    entries.clear();
    sortedEntries.clear();

    startOfEndOfCD = -1;
    if ( endOfCentralDir64 )
        delete endOfCentralDir64;
    endOfCentralDir64 = 0;
    modified = false;

    endOfCentralDir.reset( );
}

qint64 ZipCentralDir::numberOfEntries()
{
    if ( endOfCentralDir64 )
        return endOfCentralDir64->numberOfEntries;

    return endOfCentralDir.numberOfEntries;
}

void ZipCentralDir::setNumberOfEntries( qint64 num )
{
    if ( endOfCentralDir64 )
    {
        endOfCentralDir.numberOfEntries = 0xffff;
        endOfCentralDir.numberOfCdEntriesOnDisk = 0xffff;
        endOfCentralDir64->numberOfEntries = num;
        endOfCentralDir64->numberOfCdEntriesOnDisk = num;
    }
    else
    {
        endOfCentralDir.numberOfEntries = num;
        endOfCentralDir.numberOfCdEntriesOnDisk = num;
    }
}


qint64 ZipCentralDir::offsetToStartOfCD()
{
    if ( endOfCentralDir64 )
        return endOfCentralDir64->offsetToStartOfCD;

    return endOfCentralDir.offsetToStartOfCD;
}

void ZipCentralDir::setOffsetToStartOfCD( qint64 pos )
{
    if ( pos > 0xffffffff && !endOfCentralDir64 )
    {
        // Create a new Zip64 record
        endOfCentralDir64 = new EndOfCentralDir64;
    }

    if ( endOfCentralDir64 )
    {
        endOfCentralDir.offsetToStartOfCD = 0xffffffff;
        endOfCentralDir64->offsetToStartOfCD = pos;
    }
    else
    {
        endOfCentralDir.offsetToStartOfCD = pos;
    }
}

qint64 ZipCentralDir::sizeOfCD()
{
    if ( endOfCentralDir64 )
        return endOfCentralDir64->sizeOfCD;

    return endOfCentralDir.sizeOfCD;
}

void ZipCentralDir::setSizeOfCD( qint64 size )
{
    if ( endOfCentralDir64 )
    {
        endOfCentralDir.sizeOfCD = 0xffffffff;
        endOfCentralDir64->sizeOfCD = size;
    }
    else
    {
        endOfCentralDir.sizeOfCD = size;
    }
}

bool ZipCentralDir::read( QIODevice* ioDevice, AbZipPrivate* zip )
{
    if ( startOfEndOfCD < 0)
    {
        isValid = false;

        startOfEndOfCD = locateSignature( CENTRAL_DIR_END_SIGNATURE, ioDevice );

        if ( startOfEndOfCD < 0 )
        {
            return zip->setError(AbZip::CentralDirError,
                                 QT_TR_NOOP("Error locating Central Directory! Will attempt to recover it.") );
        }

        // Move to start of 'End of Central Dir'
        ioDevice->seek(startOfEndOfCD);
        if ( !endOfCentralDir.read( ioDevice ) )
        {
            return zip->setError(AbZip::CentralDirError,
                                 QT_TR_NOOP("Error reading End of Central Directory! Will attempt to recover it.") );
        }

        // Do we have a zip64 file?
        if ( endOfCentralDir.needsZip64() )
        {
            qint64 pos = locateSignature( CENTRAL_DIR_END64_LOCATOR_SIGNATURE, ioDevice  );
            if ( pos >= 0 )
            {
                ioDevice->seek(pos);
                endOfCentralDir64 = new EndOfCentralDir64;
                endOfCentralDir64->read( ioDevice );
            }
        }

        calcBytesBeforeZip();

        // 'End of Central Dir' read ok. now read the Central Directory
        ioDevice->seek( offsetToStartOfCD() );
        for( int i=0; i < numberOfEntries(); i++ )
        {
            CentralDirFileHeader* header = new CentralDirFileHeader;
            if ( header->read( ioDevice ) )
                entries.append( header );
            else
                zip->setError(AbZip::CentralDirError,
                                    QT_TR_NOOP("Invalid Central Directory header record found!") );
        }
        isValid = true;
    }
    return true;
}

qint64 ZipCentralDir::write( QIODevice* ioDevice )
{
    if ( modified && isValid )
    {
        // Make sure we are at the correct point to save the CD
        ioDevice->seek( offsetToStartOfCD() );

        // Write all the CentralDirFileHeaders
        qint64 sizeOfCD = 0;
        setNumberOfEntries( entries.count() );

        foreach( CentralDirFileHeader* header, entries )
        {
            sizeOfCD += header->write( ioDevice );
        }

        setSizeOfCD( sizeOfCD );

        // Write the Zip64 End of Central Dir
        if ( endOfCentralDir64 )
            endOfCentralDir64->write( ioDevice );

        // Now write the End of Central Dir
        return endOfCentralDir.write( ioDevice );
    }
    return 1;   // This is ok
}


void ZipCentralDir::calcBytesBeforeZip()
{
    if ( endOfCentralDir64 )
        bytesBeforeZip = startOfEndOfCD - endOfCentralDir64->sizeOfCD - endOfCentralDir64->offsetToStartOfCD;
    else
        bytesBeforeZip = startOfEndOfCD - endOfCentralDir.sizeOfCD - endOfCentralDir.offsetToStartOfCD;
}



qint64 ZipCentralDir::locateSignature( quint32 signature, QIODevice* ioDevice  )
{
    Q_ASSERT( ioDevice );

    QByteArray baSig = QByteArray::fromRawData((const char*) &signature, sizeof(quint32)) ;

    qint64 fileSize = ioDevice->size();

    int readSize = 0x400;     // max read size
    qint64 foundPos = -1;

    QByteArray ba;

    readSize = (fileSize < readSize) ? fileSize : readSize;

    qint64 readFrom = fileSize-readSize;
    while (readSize)
    {
        // Seek to start point to read buffer
        ioDevice->seek(readFrom);

        // Read a block from file
        ba.clear();
        ba = ioDevice->read( readSize );
        if ( ba.size() != readSize )
        {
            // Error reading from file
            return -1;
        }

        // Search from end of buffer as it's most likely to be closer to end if no comments etc.
        for (int i = (int)readSize-3; i > 0 ; i-- )
        {
            if ( ba[i]==baSig[0] && ba[i+1]==baSig[1] &&
                 ba[i+2]==baSig[2] && ba[i+3]==baSig[3] )
            {
                foundPos = readFrom + i;
                break;
            }
        }

        if ( foundPos >= 0 || readFrom == 0)
            break;

        // try previous block
        if ( readFrom-readSize > 0 )
            readFrom -= readSize;
        else
        {
            // we've hit the start of the file
            readSize = readFrom;
            readFrom = 0;
        }
    }

    return foundPos;
}

CentralDirFileHeader* ZipCentralDir::findFile( const QString& file, AbZip::ZipOptions options )
{

    foreach( CentralDirFileHeader* header, entries )
    {
        if ( file.compare( header->sFileName,
                           options & AbZip::CaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ) == 0 )
        {
            return header;
        }
    }
    return NULL;
}


void ZipCentralDir::adjustOffsets( qint64 fromOffset, qint64 size )
{
    qint64 offset;
    foreach( CentralDirFileHeader* header, entries )
    {
        offset = header->getRelativeOffset();
        if ( offset > fromOffset )
        {
            offset -= size;
            header->setSizes( header->getUncompressedSize(), header->getCompressedSize(), offset );
        }
    }
}

ZipInfo ZipCentralDir::getInfo()
{
    ZipInfo info;

    info.isZip64 = endOfCentralDir64 ? true : false;
    if ( endOfCentralDir64 )
    {
        info.version = LOBYTE( endOfCentralDir64->versionMadeBy );
        info.madeByOS = HIBYTE( endOfCentralDir64->versionMadeBy );
    }

    foreach( CentralDirFileHeader* header, entries )
    {
        if ( !header->isDirectory() )
            info.fileCount++;
        info.packedSize += header->getCompressedSize();
        info.unpackedSize += header->getUncompressedSize();

        // Get a versionMadeBy from one of the files (not strictly true for whole file!)
        if ( !info.version )
        {
            info.version = LOBYTE( header->versionMadeBy );
            info.madeByOS = HIBYTE( header->versionMadeBy );
        }
    }

    // If we have a zip64 then use that version
    if ( endOfCentralDir64 )
        info.version = LOBYTE(endOfCentralDir64->versionMadeBy);

    info.comment = endOfCentralDir.getComment();

    return info;
}


qint64 ZipCentralDir::createSortedHeader()
{
    if ( entries.size() > 0 && entries.size() != sortedEntries.size() )
    {
        sortedEntries.clear();

        // Create now.  QMap's are sorted automatically and the QString class already has a <() operator to do so
        foreach( CentralDirFileHeader* header, entries )
            sortedEntries.insert( header->sFileName.toLower(), header );

        // Assign this to the iterator
        iteratorEntries = sortedEntries;
    }
    return sortedEntries.size();
}

CentralDirFileHeader* ZipCentralDir::getFirstFile()
{
    createSortedHeader();       // created the sorted Entries Map of first calling

    iteratorEntries.toFront();
    if ( iteratorEntries.hasNext())
    {
        iteratorEntries.next();
        return iteratorEntries.value();
    }
    return NULL;
}

CentralDirFileHeader* ZipCentralDir::getNextFile()
{
    //createSortedHeader();   // We must call this here just incase the user adds or deleted a file during an iterator operation

    while (iteratorEntries.hasNext())
    {
        iteratorEntries.next();
        return iteratorEntries.value();
    }
    return NULL;
}

bool ZipCentralDir::hasNextFile()
{
    return iteratorEntries.hasNext();
}






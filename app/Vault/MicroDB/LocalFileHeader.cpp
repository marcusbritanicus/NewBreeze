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

#include "LocalFileHeader.h"
#include "iobuffer.h"
#include "CentralDirFileHeader.h"
#include "Compressor.h"

#include <QIODevice>
#include <QDebug>
#include <QDateTime>
#include <QTextCodec>

LocalFileHeader::LocalFileHeader(QObject *parent) : QObject(parent)
{
    signature = LOCAL_FILE_HEADER_SIGNATURE;
    versionNeeded = VERSION_NEEDED;
    generalFlag = 0;
    compressionMethod = 0;
    lastModFileTime = 0;
    lastModFileDate = 0;
    crc_32 = 0;
    compressedSize = 0;
    uncompressedSize = 0;
    filenameLength = 0;
    extraFieldLength = 0;

    // Expanded version of extraField if there are any
    extraFields = NULL;
}

LocalFileHeader::LocalFileHeader(const LocalFileHeader & other) : QObject()
{
    extraFields = NULL;

    *this = other;
}

LocalFileHeader& LocalFileHeader::operator=(const LocalFileHeader& other)
{
    // check for self-assignment
    if(&other == this)
        return *this;

    signature = other.signature;
    versionNeeded = other.versionNeeded;
    generalFlag = other.generalFlag;
    compressionMethod = other.compressionMethod;
    lastModFileTime = other.lastModFileTime;
    lastModFileDate = other.lastModFileDate;
    crc_32 = other.crc_32;
    compressedSize = other.compressedSize;
    uncompressedSize = other.uncompressedSize;
    filenameLength = other.filenameLength;
    extraFieldLength = other.extraFieldLength;
    fileName = other.fileName;
    extraField = other.extraField;

    if ( other.extraFields )
    {
        if ( !extraFields )
            extraFields = new ExtraFields;
        *extraFields = *(other.extraFields);
    }

    return *this;
}

LocalFileHeader::~LocalFileHeader()
{
    if ( extraFields )
        delete extraFields;
}

void LocalFileHeader::init( CentralDirFileHeader* header)
{
    versionNeeded = header->versionNeeded;
    generalFlag = header->generalFlag;
    compressionMethod = header->compressionMethod;
    lastModFileTime = header->lastModFileTime;
    lastModFileDate = header->lastModFileDate;
    filenameLength = header->filenameLength;
    fileName = header->fileName;
    extraFieldLength = header->extraFieldLength;
    extraField = header->extraField;

    if ( header->extraFields )
    {
        if ( !extraFields )
            extraFields = new ExtraFields;
        *extraFields = *header->extraFields;
    }
}


bool LocalFileHeader::validate( LocalFileHeader* header)
{
    // If all these fields are the same then assume headers are equal
    // This might not be correct!!
    if ( versionNeeded == header->versionNeeded &&
        generalFlag == header->generalFlag &&
        compressionMethod == header->compressionMethod &&
        lastModFileTime == header->lastModFileTime &&
        lastModFileDate == header->lastModFileDate &&
        crc_32 == header->crc_32 &&
        compressedSize == header->compressedSize &&
        uncompressedSize == header->uncompressedSize &&
        filenameLength == header->filenameLength &&
        fileName == header->fileName )
    {
        return true;
    }

    return false;
}


void LocalFileHeader::initFromFile( const QFileInfo& file )
{
    uncompressedSize = file.isDir() ? 0 : file.size();
    compressionMethod = file.isDir() ? methodStore : methodDeflate;
    versionNeeded = file.isDir() ? 10 : versionNeeded;


    QDateTime dtMod = file.lastModified();

    QDate d = dtMod.date();
    lastModFileDate = (quint16) (d.day() + ( d.month() << 5) + ((d.year() - 1980) << 9));
    QTime t = dtMod.time();
    lastModFileTime = (quint16) (((t.second() / 2) >> 1) + (t.minute() << 5) + (t.hour() << 11));

    // Lets also try to add some better file times in extra data
    if ( !extraFields )
        extraFields = new ExtraFields;

#ifdef Q_OS_WIN
    QDateTime epoch(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
    qint64 mtime = epoch.msecsTo( dtMod ) * 10000;
    qint64 atime = epoch.msecsTo( file.lastRead() ) * 10000;
    qint64 ctime = epoch.msecsTo( file.created() ) * 10000;

    extraFields->setNtfs(mtime, atime, ctime);
#else
    extraFields->setTimeStamp( dtMod.toTime_t() );
#endif

}

void LocalFileHeader::setFileName( const QString& filename )
{
    // Always store in UTF8
    fileName = QTextCodec::codecForName("UTF-8")->fromUnicode( filename );    // Save in a QString for convienience (searching etc)
    filenameLength = fileName.size();
}


// we must be at the right position in the file!
bool LocalFileHeader::read(QIODevice* ioDevice)
{
    Q_ASSERT(ioDevice);

    IOBuffer buffer;
    buffer.read( ioDevice, LOCAL_FILE_HEADER_SIZE );

    signature = buffer.readUInt32( );
    if ( signature != LOCAL_FILE_HEADER_SIGNATURE )
    {
        qWarning() << "CentralDirFileHeader::read: invalid signature";
        return false;
    }

    versionNeeded = buffer.readUInt16( );
    generalFlag = buffer.readUInt16( );
    compressionMethod = buffer.readUInt16( );
    lastModFileTime = buffer.readUInt16( );
    lastModFileDate = buffer.readUInt16( );
    crc_32 = buffer.readUInt32( );
    compressedSize = buffer.readUInt32( );
    uncompressedSize = buffer.readUInt32( );
    filenameLength = buffer.readUInt16( );
    extraFieldLength = buffer.readUInt16( );

    if (filenameLength)
        fileName = ioDevice->read( filenameLength );

    readExtraData(ioDevice);

    return isValid();
}

void LocalFileHeader::readExtraData(QIODevice* ioDevice)
{
    if (extraFieldLength)
    {
        extraField = ioDevice->read( extraFieldLength );

        // See what fields we have
        extraFields = new ExtraFields( extraField );
    }
}

qint64 LocalFileHeader::write(QIODevice* ioDevice )
{
//    Q_UNUSED( update ); // use later to only write the bits we need not the whole header
    Q_ASSERT(ioDevice);

    IOBuffer buffer;

    buffer.writeUInt32( signature );
    buffer.writeUInt16( versionNeeded );
    buffer.writeUInt16( generalFlag );
    buffer.writeUInt16( compressionMethod );
    buffer.writeUInt16( lastModFileTime );
    buffer.writeUInt16( lastModFileDate );
    buffer.writeUInt32( crc_32 );
    buffer.writeUInt32( compressedSize );
    buffer.writeUInt32( uncompressedSize );
    buffer.writeUInt16( filenameLength );

    checkForExtraData();
    buffer.writeUInt16( extraFieldLength );

    if ( buffer.size() != LOCAL_FILE_HEADER_SIZE )
    {
        qDebug() << "LocalFileHeader::write: invalid header size!";
        return -1;
    }

    if (filenameLength)
        buffer.writeRawBytes( fileName );

    if (extraFieldLength)
        buffer.writeRawBytes( extraField );

    return ioDevice->write( buffer.getByteArray() );
}

void LocalFileHeader::checkForExtraData()
{
    if ( extraFields )
    {
        // If we have extra fields then get these as we may have added some
        extraField = extraFields->write();
        extraFieldLength = extraField.size();
    }
}


qint64 LocalFileHeader::writeDataDescriptor( QIODevice* ioDevice )
{
    Q_ASSERT(ioDevice);

    IOBuffer buffer;

    buffer.writeUInt32( DATA_DESCRIPTOR_SIGNATURE );
    buffer.writeUInt32( crc_32 );
    buffer.writeUInt32( compressedSize );
    buffer.writeUInt32( uncompressedSize );

    return ioDevice->write( buffer.getByteArray() );
}

void LocalFileHeader::readDataDescriptor( QIODevice* ioDevice )
{
    Q_ASSERT(ioDevice);

    IOBuffer buffer;
    buffer.read( ioDevice, sizeof(quint32)*3 );

    quint32 signature = buffer.readUInt32( );
    if ( signature == DATA_DESCRIPTOR_SIGNATURE )
    {
        // Found signature
        crc_32 = buffer.readUInt32( );
        compressedSize = buffer.readUInt32( );

        // Read 1 more int32
        buffer.read( ioDevice, sizeof(quint32) );
        uncompressedSize = buffer.readUInt32( );
    }
    else
    {
        crc_32 = signature;
        compressedSize = buffer.readUInt32( );
        uncompressedSize = buffer.readUInt32( );
    }
}

void LocalFileHeader::setSizes( qint64 uncompSize, qint64 compSize, qint64 offset )
{
    if ( compSize > 0xffffffff || uncompSize > 0xffffffff || offset > 0xffffffff )
    {
        // save in extra data
        versionNeeded = VERSION_NEEDED_ZIP64;     // version 4.5 required for zip64
        compressedSize = 0xffffffff;
        uncompressedSize = 0xffffffff;

        // Store the sizes and offset in the zip64 extra field
        if ( !extraFields )
            extraFields = new ExtraFields( extraField );

        extraFields->setZip64( uncompSize, compSize, offset );
    }
    else
    {
        compressedSize = compSize;
        uncompressedSize = uncompSize;
    }
}

quint64 LocalFileHeader::getCompressedSize()
{
    if ( compressedSize == 0xffffffff)
    {
        qint64 comp = 0;
        if ( extraFields)
            extraFields->zip64(0, &comp);
        return comp;
    }
    return compressedSize;
}

quint64 LocalFileHeader::getUncompressedSize()
{
    if ( uncompressedSize == 0xffffffff)
    {
        qint64 uncomp = 0;
        if ( extraFields)
            extraFields->zip64(&uncomp);
        return uncomp;
    }
    return uncompressedSize;
}



QDateTime LocalFileHeader::lastModDate()
{
    // Check if there is a better file mod date in extra data
    if ( extraFields )
    {
        qint64 mtime = 0;

        // look for ntfs first
        extraFields->ntfs( &mtime );
        if ( mtime )
        {
            QDateTime dt(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
            return dt.addMSecs((quint64)mtime / 10000);
        }
        // now try timestamp
        extraFields->timestamp( &mtime );
        if ( mtime )
        {
            // Unix EPOCH is 1970-01-01T00:00:00Z
            return QDateTime::fromMSecsSinceEpoch(mtime*1000 );
        }
        // now try UNIX
        extraFields->getUnix( &mtime );
        if ( mtime )
        {
            return QDateTime::fromMSecsSinceEpoch( mtime*1000 );
        }
    }

    // Ok we have to use normal file time stamp
    QDate d((lastModFileDate >> 9) + 1980,
            ((lastModFileDate & ~0xFE00) >> 5) - 1,
            lastModFileDate & ~0xFFE0);
    QTime t(lastModFileTime >> 11,
            (lastModFileTime & ~0xF800) >> 5,
            (lastModFileTime & ~0xFFE0) << 1);


    return QDateTime( d, t );
}


bool LocalFileHeader::isDirectory( )
{
    return false;
}

bool LocalFileHeader::isNewer( LocalFileHeader* header )
{
    if ( header->getUncompressedSize() != getUncompressedSize() ||  // Size first
         header->lastModFileDate  > lastModFileDate  ||             // Date
         (header->lastModFileDate  == lastModFileDate &&            // if Date equal then check time
          header->lastModFileTime  > lastModFileTime ) )
    {
        return true;
    }

    return false;
}

ZipFileInfo LocalFileHeader::getFileInfo()
{
    return ZipFileInfo();
}


void LocalFileHeader::setWinZipAES( )
{
    if ( !extraFields )
        extraFields = new ExtraFields;      // should already be created

    extraFields->setWinZipAES( compressionMethod );

    // Switch method to WinZip AES
    compressionMethod = AES_METHOD;
}


bool LocalFileHeader::getWinZipAES( quint16& method, quint8& strength )
{
    if ( extraFields )
        return extraFields->getWinZipAES( method, strength );

    return false;
}







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

#include "CentralDirFileHeader.h"
#include "iobuffer.h"
#include "Compressor.h"
#include "Utils.h"

#include <QIODevice>
#include <QTextCodec>
#include <QDebug>
#include <QFileInfo>

using namespace ZipUtils;

CentralDirFileHeader::CentralDirFileHeader(QObject *parent) : LocalFileHeader(parent)
{
    signature = CD_FILE_HEADER_SIGNATURE;
    versionMadeBy = VERSION_MADE_BY;

    fileCommentLength = 0;
    diskNumberStart = 0;
    internalFileAttr = 0;
    externalFileAttr = 0;
    relativeOffset = 0;
}

CentralDirFileHeader::CentralDirFileHeader(const CentralDirFileHeader & other) : LocalFileHeader(other)
{
    *this = other;
}

CentralDirFileHeader& CentralDirFileHeader::operator=(const CentralDirFileHeader& other)
{
    // check for self-assignment
    if(&other == this)
        return *this;

    versionMadeBy = other.versionMadeBy;
    fileCommentLength = other.fileCommentLength;
    diskNumberStart = other.diskNumberStart;
    internalFileAttr = other.internalFileAttr;
    externalFileAttr = other.externalFileAttr;
    relativeOffset = other.relativeOffset;
    comment = other.comment;
    sFileName = other.sFileName;

    return *this;
}


CentralDirFileHeader::~CentralDirFileHeader()
{
}


// we must be at the right position in the file!
bool CentralDirFileHeader::read(QIODevice* ioDevice)
{
    Q_ASSERT(ioDevice);

    IOBuffer buffer;
    buffer.read( ioDevice, CD_FILE_HEADER_SIZE );

    signature = buffer.readUInt32( );      // 0x02014b50
    if ( signature != CD_FILE_HEADER_SIGNATURE )
        return false;

    versionMadeBy = buffer.readUInt16( );
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
    fileCommentLength = buffer.readUInt16( );
    diskNumberStart = buffer.readUInt16( );
    internalFileAttr = buffer.readUInt16( );
    externalFileAttr = buffer.readUInt32( );
    relativeOffset = buffer.readUInt32( );

    if (filenameLength)
    {
        fileName = ioDevice->read( filenameLength );
        if ( isUTF8() )
            sFileName = QTextCodec::codecForUtfText(fileName)->toUnicode( fileName );    // Save in a QString for convienience (searching etc)
        else
            sFileName = QTextCodec::codecForLocale()->toUnicode( fileName );    // Save in a QString for convienience (searching etc)
    }

    readExtraData( ioDevice );

    if (fileCommentLength)
        comment = ioDevice->read( fileCommentLength );

    return isValid();
}

qint64 CentralDirFileHeader::write(QIODevice* ioDevice)
{
    Q_ASSERT(ioDevice);

    filenameLength  = fileName.size();
    extraFieldLength = extraField.size();
    fileCommentLength = comment.size();

    IOBuffer buffer;

    buffer.writeUInt32( signature );
    buffer.writeUInt16( versionMadeBy );
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
    buffer.writeUInt16( fileCommentLength );
    buffer.writeUInt16( diskNumberStart );
    buffer.writeUInt16( internalFileAttr );
    buffer.writeUInt32( externalFileAttr );
    buffer.writeUInt32( relativeOffset );

    if ( buffer.size() != CD_FILE_HEADER_SIZE )
    {
        qDebug() << "CentralDirFileHeader::write: invalid header size!";
        return 0;
    }

    if (filenameLength)
        buffer.writeRawBytes( fileName );

    if (extraFieldLength)
        buffer.writeRawBytes( extraField );

    if (fileCommentLength)
        buffer.writeRawBytes( comment );

    return ioDevice->write( buffer.getByteArray() );
}


void CentralDirFileHeader::initFromFile( const QFileInfo& file )
{
    LocalFileHeader::initFromFile( file );

    setPermissions( file.permissions(), file.isDir() );

    // Windows Only: Add the files attributes to externalAttr (e.g. archive, readonly, system etc.)
    quint32 attr = getFileAttributes( file.filePath() );

    if ( attr & 0x20 )  // Archive
        externalFileAttr |= 0x20;
    if ( attr & 0x10 )  // Folder
        externalFileAttr |= 0x10;
    if ( attr & 0x08 )  // System
        externalFileAttr |= 0x08;
    if ( attr & 0x02 )  // Hidden
        externalFileAttr |= 0x02;
    if ( attr & 0x01 )  // ReadOnly
        externalFileAttr |= 0x01;
}

// This is call when repairing a damaged or lost CD
// We need to make some assumptions about the external and internal attributes!!
void CentralDirFileHeader::initFromLocalHeader( const LocalFileHeader& localHeader )
{
    // Call copy operator for local Header
    *((LocalFileHeader*)this) = localHeader;

    if ( getCompressedSize() == 0 )
    {
        // it's a dir
        setPermissions( 0, true );
    }
    else
    {
        // default all files to 'Archive' and RW
        setPermissions( QFile::ReadOwner | QFile::WriteOwner |
                        QFile::ReadGroup | QFile::WriteGroup |
                        QFile::ReadOther | QFile::WriteOther, false);
        externalFileAttr |= 0x20;
    }
}

void CentralDirFileHeader::setSizes( qint64 uncompSize, qint64 compSize, qint64 offset)
{
    if ( compSize > 0xffffffff || uncompSize > 0xffffffff || offset > 0xffffffff)
    {
        versionMadeBy =  MAKESHORT( VERSION_NEEDED_ZIP64, getOsMadeBy() )  ;
        relativeOffset = 0xffffffff;
    }
    else
    {
        relativeOffset = offset;
    }

    LocalFileHeader::setSizes( uncompSize, compSize, offset );
}

// returns the relative offset for either 32 or 64 bit versions
quint64 CentralDirFileHeader::getRelativeOffset()
{
    if ( relativeOffset == 0xffffffff)
    {
        qint64 offset = 0;
        if ( extraFields)
            extraFields->zip64(0, 0, &offset);
        return offset;
    }
    return relativeOffset;
}


void CentralDirFileHeader::setFileName( const QString& filename )
{
    sFileName = filename;

    // Always store in UTF8
    fileName = QTextCodec::codecForName("UTF-8")->fromUnicode( filename );    // Save in a QString for convienience (searching etc)
    filenameLength = fileName.size();

    //generalFlag |= ZIP_IS_UTF8;
}

//TTTTsstrwxrwxrwx0000000000ADVSHR
//^^^^____________________________ file type - x1000000 file, x400000 dir
//    ^^^_________________________ setuid, setgid, sticky
//       ^^^^^^^^^________________ permissions
//                ^^^^^^^^________ This is the "lower-middle byte" your post mentions
//                        ^^^^^^^^ DOS attribute bits
void CentralDirFileHeader::setPermissions( QFile::Permissions perm, bool isDir)
{
    quint32 attr = isDir ? 0x4000 : 0x8000;

    if ((perm & QFile::ReadOwner) != 0)
        attr |= 0x100;
    if ((perm & QFile::WriteOwner) != 0)
        attr |= 0x80;
    if ((perm & QFile::ExeOwner) != 0)
        attr |= 0x40;
    if ((perm & QFile::ReadGroup) != 0)
        attr |= 0x20;
    if ((perm & QFile::WriteGroup) != 0)
        attr |= 0x10;
    if ((perm & QFile::ExeGroup) != 0)
        attr |= 0x08;
    if ((perm & QFile::ReadOther) != 0)
        attr |= 0x04;
    if ((perm & QFile::WriteOther) != 0)
        attr |= 0x02;
    if ((perm & QFile::ExeOther) != 0)
        attr |= 0x01;

    externalFileAttr = (attr << 16);

    if ( isDir )
        externalFileAttr |= 0x10;
}

QFile::Permissions  CentralDirFileHeader::getPermissions()
{
    quint32 attr = (externalFileAttr >> 16) & 0xffff;

    QFile::Permissions perm = 0;

    if ((attr & 0x100) != 0)
        perm |= QFile::ReadOwner;
    if ((attr & 0x80) != 0)
        perm |= QFile::WriteOwner;
    if ((attr & 0x40) != 0)
        perm |= QFile::ExeOwner;
    if ((attr & 0x20) != 0)
        perm |= QFile::ReadGroup;
    if ((attr & 0x10) != 0)
        perm |= QFile::WriteGroup;
    if ((attr & 0x08) != 0)
        perm |= QFile::ExeGroup;
    if ((attr & 0x04) != 0)
        perm |= QFile::ReadOther;
    if ((attr & 0x02) != 0)
        perm |= QFile::WriteOther;
    if ((attr & 0x01) != 0)
        perm |= QFile::ExeOther;
    return perm;

}

bool CentralDirFileHeader::isDirectory( )
{
    quint32 attr = (externalFileAttr >> 16) & 0xffff;

    return ((externalFileAttr&0x10) || (attr & 0x4000)) ? true : false;
}


ZipFileInfo CentralDirFileHeader::getFileInfo()
{
    return ZipFileInfo( this );
}



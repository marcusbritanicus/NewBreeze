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

#include "EndOfCentralDir.h"
#include "iobuffer.h"
#include "Utils.h"

#include <QTextCodec>
#include <QIODevice>
#include <QDebug>


EndOfCentralDir::EndOfCentralDir( )
{
    signature = CENTRAL_DIR_END_SIGNATURE;
    reset( );
}


void EndOfCentralDir::reset( )
{
    numberOfThisDisk = 0;
    numberOfDiskCdStarts = 0;
    numberOfCdEntriesOnDisk = 0;
    numberOfEntries = 0;
    sizeOfCD = 0;
    offsetToStartOfCD = 0;
    commentLength = 0;
    comment.clear();
}


// we must be at the right position in the file!
bool EndOfCentralDir::read(QIODevice* ioDevice)
{
    Q_ASSERT(ioDevice);

    IOBuffer buffer;
    buffer.read( ioDevice, CENTRAL_DIR_END_SIZE );

    signature = buffer.readUInt32( );
    numberOfThisDisk =  buffer.readUInt16( );
    numberOfDiskCdStarts = buffer.readUInt16( );
    numberOfCdEntriesOnDisk = buffer.readUInt16( );
    numberOfEntries = buffer.readUInt16( );
    sizeOfCD = buffer.readUInt32( );
    offsetToStartOfCD = buffer.readUInt32( );
    commentLength = buffer.readUInt16( );

    if (commentLength)
        comment = ioDevice->read( commentLength );

    return isValid();
}


qint32 EndOfCentralDir::write(QIODevice* ioDevice)
{
    Q_ASSERT(ioDevice);

    commentLength = comment.size();

    IOBuffer buffer;

    buffer.writeUInt32( signature );      // 0x02014b50
    buffer.writeUInt16( numberOfThisDisk );
    buffer.writeUInt16( numberOfDiskCdStarts );
    buffer.writeUInt16( numberOfCdEntriesOnDisk );
    buffer.writeUInt16( numberOfEntries );
    buffer.writeUInt32( sizeOfCD );
    buffer.writeUInt32( offsetToStartOfCD );
    buffer.writeUInt16( commentLength );

    if (commentLength)
        buffer.writeRawBytes( comment );

    return ioDevice->write( buffer.getByteArray() );

}

QString EndOfCentralDir::getComment(  )
{
    QString cmt;
    if (commentLength)
    {
         cmt = QTextCodec::codecForUtfText(comment)->toUnicode( comment );
    }
    return cmt;
}

void EndOfCentralDir::setComment( const QString& cmt )
{
    comment = QTextCodec::codecForName("UTF-8")->fromUnicode( cmt );
    commentLength = comment.size();
}


EndOfCentralDir64::EndOfCentralDir64( )
{
    signature = CENTRAL_DIR_END64_SIGNATURE;
    sizeOfzip64EndOfCDRecord = 0;
    versionMadeBy = MAKESHORT( 45, ZipUtils::getOsMadeBy() );     // version 4.5 for Zip64
    versionNeeded = 45;     // version 4.5 for Zip64
    numberOfThisDisk = 0;
    numberOfDiskCdStarts = 0;
    numberOfCdEntriesOnDisk = 0;
    numberOfEntries = 0;
    sizeOfCD = 0;
    offsetToStartOfCD = 0;
}

// we must be at the right position in the file!
bool EndOfCentralDir64::read(QIODevice* ioDevice)
{
    IOBuffer buffer;

    // Read the Zip64 end of central dir locater
    buffer.read( ioDevice, CENTRAL_DIR_END64_LOCATOR_SIZE );

    quint32 sig = buffer.readUInt32( );       // skip the signature ( 0x07064b50 )
    if (sig != CENTRAL_DIR_END64_LOCATOR_SIGNATURE)
        return false;

    buffer.readUInt32( );              // number of the disk with the start of the zip64 end of  central directory
    quint64 relativeOffset = buffer.readUInt64( );          // relative offset of the zip64 end of central directory record
    buffer.readUInt32( );             // total number of disks (should be 1)

    // Seek to the start of the zip64 end of central dir record
    ioDevice->seek( relativeOffset );

    // Read the 'Zip64 end of central dir'
    buffer.read( ioDevice, CENTRAL_DIR_END64_SIZE );
    signature = buffer.readUInt32( );             // signature of 'Zip64 end of central dir'
    if (signature != CENTRAL_DIR_END64_SIGNATURE)
      return false;

    sizeOfzip64EndOfCDRecord = buffer.readUInt64( );     //size of zip64 end of central directory record
    versionMadeBy =  buffer.readUInt16( );
    versionNeeded =  buffer.readUInt16( );
    numberOfThisDisk =  buffer.readUInt32( );
    numberOfDiskCdStarts =  buffer.readUInt32( );
    numberOfCdEntriesOnDisk =  buffer.readUInt64( );
    numberOfEntries =  buffer.readUInt64( );
    sizeOfCD =  buffer.readUInt64( );
    offsetToStartOfCD =  buffer.readUInt64( );

    // Read the extensible data sector
    sig = sizeOfzip64EndOfCDRecord - (CENTRAL_DIR_END64_SIZE - 12);
    if ( (qint64)sig > 0 )
        extensibleData = buffer.readByteArray( sig );

    return isValid();

}

qint32 EndOfCentralDir64::write(QIODevice* ioDevice)
{
    qint64 relativeOffset = ioDevice->pos();

    // Write the 'Zip64 end of central dir'
    IOBuffer buffer;
    buffer.writeUInt32( signature );      // 0x02014b50
    buffer.writeUInt64( sizeOfzip64EndOfCDRecord );
    buffer.writeUInt16( versionMadeBy );
    buffer.writeUInt16( versionNeeded );
    buffer.writeUInt32( numberOfThisDisk );
    buffer.writeUInt32( numberOfDiskCdStarts );
    buffer.writeUInt64( numberOfCdEntriesOnDisk );
    buffer.writeUInt64( numberOfEntries );
    buffer.writeUInt64( sizeOfCD );
    buffer.writeUInt64( offsetToStartOfCD );

    if ( extensibleData.size() > 0 )
        buffer.writeByteArray( extensibleData );

    qint64 written = ioDevice->write( buffer.getByteArray() );

    // Write the Zip64 end of central dir locater
    buffer.clear();

    buffer.writeUInt32( CENTRAL_DIR_END64_LOCATOR_SIGNATURE );
    buffer.writeUInt32( 0 );
    buffer.writeUInt64( relativeOffset );   // Offset to start of Zip64 end of CD
    buffer.writeUInt32( 1 );    // Number of disks

    written += ioDevice->write( buffer.getByteArray() );

    return written;
}

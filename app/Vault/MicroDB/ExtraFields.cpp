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

#include "ExtraFields.h"
#include "iobuffer.h"

ExtraFields::ExtraFields()
{
}

ExtraFields::ExtraFields( const QByteArray& fields )
{
    read( fields );
}


ExtraFields& ExtraFields::operator=(const ExtraFields& other)
{
    // check for self-assignment
    if(&other == this)
        return *this;

    mapFields = other.mapFields;

    return *this;
}


bool ExtraFields::read( const QByteArray& fields )
{
    IOBuffer buffer(fields);

    while( buffer.hasData() )
    {
        quint16 headerID = buffer.readUInt16();
        quint16 dataSize = buffer.readUInt16();
        if ( dataSize > 0 )
        {
            mapFields.insert( headerID, buffer.readByteArray( dataSize ) );
        }
    }
    return true;
}

QByteArray ExtraFields::write( )
{
    IOBuffer buffer;

    foreach( quint16 headerID, mapFields.keys() )
    {
        const QByteArray& ba = mapFields.value(headerID);
        buffer.writeUInt16( headerID );
        buffer.writeUInt16( (quint16)ba.size() );
        buffer.writeByteArray( ba );
    }

    return buffer.getByteArray();
}

bool ExtraFields::add( quint16 headerID, const QByteArray& data )
{
    bool exists = mapFields.contains( headerID );

    mapFields.insert( headerID, data );

    return exists;
}


// Get the things we are interested in
bool ExtraFields::zip64( qint64* uncompressSize, qint64* compressSize, qint64* offset )
{
    quint16 headerID = ZIP_EXTRA_ZIP64;

    if ( mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        if ( uncompressSize )
            *uncompressSize = buffer.readUInt64();
        else
            buffer.skip(8);

        if ( compressSize )
            *compressSize = buffer.readUInt64();
        else
            buffer.skip(8);

        if ( offset )
            *offset = buffer.readUInt64();
        else
            buffer.skip(8);

        // Don't bother with 'number of the starting disk'

        return true;
    }

    return false;
}

void ExtraFields::setZip64( qint64 uncompressSize, qint64 compressSize, qint64 offset )
{
    IOBuffer buffer;

    buffer.writeUInt64(uncompressSize);
    buffer.writeUInt64(compressSize);
    buffer.writeUInt64(offset);
    buffer.writeUInt32(0);

    mapFields.insert( ZIP_EXTRA_ZIP64, buffer.getByteArray() );
}

bool ExtraFields::ntfs( qint64* mtime, qint64* atime, qint64* ctime )
{
    quint16 headerID = ZIP_EXTRA_NTFS;

    if ( mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        buffer.readUInt32();    // Reserved
        while( buffer.hasData() )
        {
            quint16 tag = buffer.readUInt16();
            quint16 tagSize = buffer.readUInt16();
            if ( tag == 0x0001 )
            {
                if ( mtime )
                    *mtime = buffer.readUInt64();
                else
                    buffer.skip(8);

                if ( atime )
                    *atime = buffer.readUInt64();
                else
                    buffer.skip(8);

                if ( ctime )
                    *ctime = buffer.readUInt64();
                return true;
            }
            else
            {
                buffer.skip(tagSize);   // Don't want this tag
            }
        }
    }

    return false;
}

// This assumes there is only an NTFS time tag!!
void ExtraFields::setNtfs( qint64 mtime, qint64 atime, qint64 ctime )
{
    IOBuffer buffer;

    buffer.writeUInt32(0);          // Reserved!

    buffer.writeUInt16( 0x0001 );   // Tag
    buffer.writeUInt16( 24 );       // Tag size ( 3 x 8 bytes)
    buffer.writeUInt64(mtime);
    buffer.writeUInt64(atime);
    buffer.writeUInt64(ctime);

    mapFields.insert( ZIP_EXTRA_NTFS, buffer.getByteArray() );
}

bool ExtraFields::timestamp( qint64* mtime, qint64* atime, qint64* ctime )
{
    quint16 headerID = ZIP_EXTRA_TIMESTAMP;

    if ( mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        qint8 flags = buffer.readByte();    // Info bits

        if ( flags & 0x01 && buffer.hasData() )
        {
            if ( mtime )
                *mtime = buffer.readUInt32();
            else
                buffer.skip(4);
        }

        // These might no be present in Central Header File
        if ( flags & 0x02 && buffer.hasData() )
        {
            if ( atime )
                *atime = buffer.readUInt32();
            else
                buffer.skip(4);
        }

        if ( flags & 0x04 && buffer.hasData() )
        {
            if ( ctime )
                *ctime = buffer.readUInt32();
            else
                buffer.skip(4);
        }
        return true;
    }

    return false;
}

void ExtraFields::setTimeStamp( qint64 mtime, qint64 atime, qint64 ctime )
{
    IOBuffer buffer;

    qint8 flags = mtime ? 0x01 : 0;
    if ( atime )
        flags |= 0x2;
    if ( ctime )
        flags |= 0x4;

    buffer.writeByte(flags);          // Info bits!
    buffer.writeUInt32(mtime);
    if ( atime )
        buffer.writeUInt32(atime);
    if ( ctime )
        buffer.writeUInt32(ctime);

    mapFields.insert( ZIP_EXTRA_TIMESTAMP, buffer.getByteArray() );
}


bool ExtraFields::getUnix( qint64* mtime, qint64* atime )
{
    quint16 headerID = ZIP_EXTRA_UNIX;

    if ( mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        if ( mtime )
            *mtime = buffer.readUInt32();
        else
            buffer.skip(4);

        if ( atime )
            *atime = buffer.readUInt32();
        return true;
    }

    return false;
}

void ExtraFields::setUnix( qint64 mtime, qint64 atime )
{
    IOBuffer buffer;

    buffer.writeUInt32(mtime);
    buffer.writeUInt32(atime);
    buffer.writeUInt16(0);
    buffer.writeUInt16(0);

    mapFields.insert( ZIP_EXTRA_UNIX, buffer.getByteArray() );
}



bool ExtraFields::unicode( QString& name, quint16 headerID )
{
    if ( ( headerID == ZIP_EXTRA_UNICODE_PATH || headerID == ZIP_EXTRA_UNICODE_COMMENT ) &&
        mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        buffer.readByte();     // Version, should be 1
        buffer.readUInt32();    // CRC

        name = QString::fromUtf8( buffer.readByteArray( buffer.size() - 5 ) );
        return true;
    }

    return false;
}

bool ExtraFields::getWinZipAES( quint16& method, quint8& strength )
{
    quint16 headerID = ZIP_EXTRA_WINZIP_AES;

    if ( mapFields.contains( headerID ) )
    {
        // Found...
        IOBuffer buffer( mapFields.value(headerID) );

        quint16 value = buffer.readUInt16();            // Version number for the vendor
        if ( value != 1 && value != 2 )
            return false;
        value = buffer.readByte();
        if ( (char)value != 'A' )
            return false;
        value = buffer.readByte();
        if ( (char)value != 'E' )
            return false;
        strength = buffer.readByte();
        method = buffer.readUInt16();
        return true;
    }

    return false;
}

void ExtraFields::setWinZipAES( quint16 method )
{
    IOBuffer buffer;

    buffer.writeUInt16(0x0001);     // Version number for the vendor
    buffer.writeByte( 'A' );        // Vendor ID  'AE'
    buffer.writeByte( 'E' );        // Vendor ID  'AE'
    buffer.writeByte( 0x03 );       // Enc strength 3 = 256, 2 = 192, 1 = 128
    buffer.writeUInt16( method );   // compression method
    mapFields.insert( ZIP_EXTRA_WINZIP_AES, buffer.getByteArray() );
}



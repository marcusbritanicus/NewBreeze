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

#include "StoreCompressor.h"

#include <zlib.h>

StoreCompressor::StoreCompressor(  QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    inBuffer = new char[DEFAULT_BUFSIZE];
}


bool StoreCompressor::compressData( Encryption* encrypt )
{
    qint64 toRead = inDevice->size();

    qint64 rep = toRead / DEFAULT_BUFSIZE;
    qint64 rem = toRead % DEFAULT_BUFSIZE;
    qint64 cur = 0;

    compressedSize = 0;
    crc = crc32(0L, Z_NULL, 0);

    // extract data
    qint64 read;

    while ( (read = inDevice->read(inBuffer, cur < rep ? DEFAULT_BUFSIZE : rem)) > 0 )
    {
        checkIfBinary( inBuffer, read );

        crc = crc32(crc, (unsigned char*)inBuffer, read);

        if (encrypt)
            encrypt->encryptBytes( inBuffer, read);

        compressedSize += read;
        uncompressedSize += read;
        if ( outDevice )
        {
            if (outDevice->write(inBuffer, read) != read)
                return setError(WriteFailed, "error writing to output device");
        }

        cur++;
        if ((qint64)compressedSize == toRead)
            break;
    }

    if (read < 0)
        return setError(ReadFailed, "error reading from input device");

    return true;
}



bool StoreCompressor::decompressData( qint64 toRead, Encryption* encrypt )
{
    qint64 rep = toRead / DEFAULT_BUFSIZE;
    qint64 rem = toRead % DEFAULT_BUFSIZE;
    qint64 cur = 0;

    uncompressedSize = 0;
    crc = crc32(0L, Z_NULL, 0);

    // extract data
    qint64 read;

    while ( (read = inDevice->read(inBuffer, cur < rep ? DEFAULT_BUFSIZE : rem)) > 0 )
    {
        if (encrypt)
            encrypt->decryptBytes( inBuffer, read);

        crc = crc32(crc, (unsigned char*)inBuffer, read);

        compressedSize += read;
        uncompressedSize += read;
        if ( outDevice )
        {
            if (outDevice->write(inBuffer, read) != read)
                return setError(WriteFailed, "error writing to output device");
        }

        cur++;
        if ((qint64)uncompressedSize == toRead)
            break;
    }

    if (read < 0)
        return setError(ReadFailed, "error reading from input device");

    return true;
}


bool StoreCompressor::copyData( qint64 toRead )
{
    qint64 rep = toRead / DEFAULT_BUFSIZE;
    qint64 rem = toRead % DEFAULT_BUFSIZE;
    qint64 cur = 0;

    uncompressedSize = 0;
    qint64 read;
    while ( (read = inDevice->read(inBuffer, cur < rep ? DEFAULT_BUFSIZE : rem)) > 0 )
    {
        uncompressedSize += read;
        if (outDevice->write(inBuffer, read) != read)
            return setError(WriteFailed, "error writing to output device");

        cur++;
        if ((qint64)uncompressedSize == toRead)
            break;
    }

    if (read < 0)
        return setError(ReadFailed, "error reading from input device");

    return true;
}

bool StoreCompressor::copyDataInSameFile( qint64 readPos, qint64 writePos, qint64 toRead )
{
    qint64 rep = toRead / DEFAULT_BUFSIZE;
    qint64 rem = toRead % DEFAULT_BUFSIZE;
    qint64 cur = 0;


    uncompressedSize = 0;
    qint64 read;
    inDevice->seek( readPos );
    while ( (read = inDevice->read(inBuffer, cur < rep ? DEFAULT_BUFSIZE : rem)) > 0 )
    {
        // Save current read pos
        readPos = inDevice->pos();

        uncompressedSize += read;

        // Goto the write pos
        outDevice->seek( writePos );
        if (outDevice->write(inBuffer, read) != read)
            return setError(WriteFailed, "error writing to output device");
        // Save current write pos
        writePos = outDevice->pos();

        cur++;
        if ((qint64)uncompressedSize == toRead)
            break;

        // Go back to current read pos and read next chunk
        inDevice->seek( readPos );
    }

    if (read < 0)
        return setError(ReadFailed, "error reading from input device");

    // Make sure we are end of the write pos
    outDevice->seek( writePos );
    return true;
}

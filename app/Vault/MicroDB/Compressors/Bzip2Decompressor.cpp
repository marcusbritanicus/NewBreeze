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

#ifdef USE_BZIP2

#include "Bzip2Decompressor.h"

#include <zlib.h>

Bzip2Decompressor::Bzip2Decompressor(  QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    init( );
}


bool Bzip2Decompressor::init( )
{
    memset( &bstream, 0, sizeof(bz_stream));

    if ( BZ2_bzDecompressInit(&bstream, 0, 0) != BZ_OK )
        return false;

    // Create the read/write buffers
    inBuffer = new char[DEFAULT_BUFSIZE];
    outBuffer = new char[DEFAULT_BUFSIZE];


    bstream.avail_out = DEFAULT_BUFSIZE;
    bstream.next_out = outBuffer;

    return true;
}


bool Bzip2Decompressor::decompressData( qint64 compSize, Encryption* encrypt )
{
    qint64 rep = compSize / DEFAULT_BUFSIZE;
    qint64 rem = compSize % DEFAULT_BUFSIZE;
    qint64 cur = 0;

    // extract data
    qint64 read;
    int zret;

    int decompressedSize;

    if ( !inBuffer )
    {
        // Init failed
        return setError(ZlibInitFailed, "BZIP2 Decompression Init failed!");
    }

    // Decompress until deflate stream ends or end of file
    do
    {
        read = inDevice->read(inBuffer, cur < rep ? DEFAULT_BUFSIZE : rem);
        if (!read)
            break;

        if (read < 0)
        {
            end();
            return setError(ReadFailed, "error reading from input device");
        }

        // Decrypt the data
        if ( encrypt )
            encrypt->decryptBytes( inBuffer, read);

        cur++;
        compressedSize += read;

        bstream.next_in = inBuffer;
        bstream.avail_in = read;

        do {
            bstream.avail_out = DEFAULT_BUFSIZE;
            bstream.next_out = outBuffer;

            zret = BZ2_bzDecompress(&bstream );

            switch (zret)
            {
            case BZ_DATA_ERROR:
            case BZ_MEM_ERROR:
                end();
                return setError(WriteFailed, "BZIP2 decompression error!");
                return false;
            default:
                break;
            }

            decompressedSize = DEFAULT_BUFSIZE - bstream.avail_out;
            uncompressedSize += decompressedSize;

            if (outDevice)      // we allow this to be 0 to just check the validity of the data
            {
                // Write to output device
                if (outDevice->write(outBuffer, decompressedSize) != decompressedSize)
                {
                    end();
                    return setError(WriteFailed, "error writing to output device");
                }
            }

            crc = crc32(crc, (unsigned char*)outBuffer, decompressedSize);

        } while (bstream.avail_out == 0);

    } while (zret != BZ_STREAM_END);

    end();

    return true;
}


bool Bzip2Decompressor::end()
{
    bstream.total_in_lo32 = 0;
    bstream.total_in_hi32 = 0;

    if ( BZ2_bzDecompressEnd(&bstream) != BZ_OK)
        return setError(ZlibEndFailed, "BZIP2 Decompression End failed!");

    return true;
}

#endif

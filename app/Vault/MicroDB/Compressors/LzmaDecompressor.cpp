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

#ifdef USE_LZMA

#include "LzmaDecompressor.h"

#include <zlib.h>

// read/write buffer size
#define LZMA_BUFSIZE  409600


LzmaDecompressor::LzmaDecompressor(  QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    init( );
}


bool LzmaDecompressor::init( )
{
    memset(&stream, 0, sizeof(stream));

    const uint32_t flags = LZMA_TELL_UNSUPPORTED_CHECK | LZMA_CONCATENATED;
    const uint64_t memory_limit = UINT64_MAX; /* no memory limit */

    if (lzma_stream_decoder (&stream, memory_limit, flags) != LZMA_OK)
        return false;


    // Create the read/write buffers
    inBuffer = new char[LZMA_BUFSIZE];
    outBuffer = new char[LZMA_BUFSIZE];

    return true;
}


bool LzmaDecompressor::decompressData( qint64 compSize, Encryption* encrypt )
{
    qint64 rep = compSize / LZMA_BUFSIZE;
    qint64 rem = compSize % LZMA_BUFSIZE;
    qint64 cur = 0;

    // extract data
    qint64 read;
    lzma_ret zret;
    lzma_action action = LZMA_RUN;

    int decompressedSize;

    if ( !inBuffer )
    {
        // Init failed
        return setError(ZlibInitFailed, "BZIP2 Decompression Init failed!");
    }

    // Decompress until deflate stream ends or end of file
    do
    {
        read = inDevice->read(inBuffer, cur < rep ? LZMA_BUFSIZE : rem);
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

        // Have we read everything
        if ( cur >= rep )
            action = LZMA_FINISH;

        cur++;
        compressedSize += read;


        stream.next_in = (uint8_t*)inBuffer;
        stream.avail_in = read;

        do {
            stream.avail_out = LZMA_BUFSIZE;
            stream.next_out = (uint8_t*)outBuffer;

            //zret = inflate(&stream, Z_NO_FLUSH);
            zret = lzma_code (&stream, action ); //LZMA_FINISH);

            switch (zret)
            {
            //case Z_NEED_DICT:
            case LZMA_DATA_ERROR:
            case LZMA_MEM_ERROR:
                end();
                return setError(WriteFailed, "LZMA decompression error!");
            default:
                break;
            }

            decompressedSize = LZMA_BUFSIZE - stream.avail_out;
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

            crc = crc32(crc, (const Bytef*) outBuffer, decompressedSize);

        } while (stream.avail_out == 0);

    } while (zret != LZMA_STREAM_END);
    end();

    return true;
}


bool LzmaDecompressor::end()
{
    lzma_end (&stream);
    return true;
}

#endif

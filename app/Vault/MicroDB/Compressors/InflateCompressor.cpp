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

#include "InflateCompressor.h"


InflateCompressor::InflateCompressor( QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    init(  );
}


bool InflateCompressor::init(  )
{
    stream.zalloc = (alloc_func) NULL;
    stream.zfree = (free_func) NULL;
    stream.opaque = NULL;

    if (inflateInit2(&stream, -MAX_WBITS) != Z_OK)
        return false;

    // Create the read/write buffers
    inBuffer = new char[DEFAULT_BUFSIZE];
    outBuffer = new char[DEFAULT_BUFSIZE];

    return true;
}


bool InflateCompressor::decompressData( qint64 compSize, Encryption* encrypt )
{
    qint64 rep = compSize / DEFAULT_BUFSIZE;
    qint64 rem = compSize % DEFAULT_BUFSIZE;
    qint64 cur = 0;

    // extract data
    qint64 read;
    int zret;
    int decompressedSize;

    if ( !inBuffer )
        return setError(ZlibInitFailed, QString::fromLocal8Bit(stream.msg));


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

        stream.next_in = (Bytef*) inBuffer;
        stream.avail_in = (uInt) read;

        // Run inflate() on input until output buffer not full
        do {
            stream.avail_out = DEFAULT_BUFSIZE;
            stream.next_out = (Bytef*) outBuffer;

            zret = inflate(&stream, Z_NO_FLUSH);

            switch (zret)
            {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                end();
                return setError(WriteFailed, QString::fromLocal8Bit(stream.msg));
            default:
                break;
            }

            decompressedSize = DEFAULT_BUFSIZE - stream.avail_out;
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

    } while (zret != Z_STREAM_END);

    end();

    return true;
}




bool InflateCompressor::end()
{
    if (inflateEnd(&stream) != Z_OK)
        return setError(ZlibEndFailed, QString::fromLocal8Bit(stream.msg));

    return true;
}

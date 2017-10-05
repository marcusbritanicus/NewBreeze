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

#include "Bzip2Compressor.h"

#include <zlib.h>

Bzip2Compressor::Bzip2Compressor( int compressionLevel, QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    memset( &bstream, 0, sizeof(bz_stream));

    init( compressionLevel );
}


bool Bzip2Compressor::init( int level )
{
    if (level < 1 || level > 9)
        level = 8;

    if ( BZ2_bzCompressInit(&bstream, level, 0,35) != BZ_OK )
        return false;

    // Create the read/write buffers
    inBuffer = new char[DEFAULT_BUFSIZE];
    outBuffer = new char[DEFAULT_BUFSIZE];

    return true;
}

bool Bzip2Compressor::compressData( Encryption* encrypt )
{
    qint64 read = 0;
    qint64 written = 0;

    qint64 toRead = inDevice->size();
    int flush = BZ_FLUSH;

    if ( !inBuffer )
        return setError(ZlibInitFailed, "BZIP2 Compression Init failed!");

    compressedSize = 0;
    crc = crc32(0L, Z_NULL, 0);

    qint64 compressed;
    int zret = BZ_OK;

    do
    {
        read = inDevice->read(inBuffer, DEFAULT_BUFSIZE);
        uncompressedSize += read;
        if (!read)
            break;

        if (read < 0)
        {
            end();
            return setError(ReadFailed, "error reading from input device");
        }

        checkIfBinary( inBuffer, read );

        crc = crc32(crc, (unsigned char*)inBuffer, read);

        bstream.next_in = inBuffer;
        bstream.avail_in = read;
        bstream.next_out = outBuffer;
        bstream.avail_out = DEFAULT_BUFSIZE;

        flush = ((qint64)uncompressedSize == toRead) ? BZ_FINISH : BZ_FLUSH;

        zret = BZ2_bzCompress(&bstream, flush);

        compressed = DEFAULT_BUFSIZE - bstream.avail_out;

        // Encrypt buffer if needed
        if (encrypt)
            encrypt->encryptBytes( outBuffer, compressed);

        compressedSize += compressed;
        if ( outDevice )
        {
            written = outDevice->write(outBuffer, compressed);
            if (written != compressed)
            {
                end();
                return setError(WriteFailed, "error writing to output device");
            }
        }

    } while (zret != BZ_STREAM_END );

    end();

    return true;
}


bool Bzip2Compressor::end()
{
    if ( BZ2_bzCompressEnd(&bstream) != BZ_OK)
        return setError(ZlibEndFailed, "BZIP2 Compression End failed!");

    return true;
}

#endif

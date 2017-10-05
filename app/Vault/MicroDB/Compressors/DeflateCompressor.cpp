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

#include "DeflateCompressor.h"

DeflateCompressor::DeflateCompressor( int compressionLevel, QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{
    stream.zalloc = Z_NULL;
    stream.zfree  = Z_NULL;
    stream.opaque = Z_NULL;
    init( compressionLevel );
}


bool DeflateCompressor::init( int level )
{
    if (level < -1 || level > 9)
        level = Z_DEFAULT_COMPRESSION;

    if (deflateInit2(&stream, level, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        return false;

    // Create the read/write buffers
    inBuffer = new char[DEFAULT_BUFSIZE];
    outBuffer = new char[DEFAULT_BUFSIZE];

    return true;
}

bool DeflateCompressor::compressData( Encryption* encrypt )
{
    qint64 read = 0;
    qint64 written = 0;

    qint64 toRead = inDevice->size();

    if ( !inBuffer )
        return setError(ZlibInitFailed, QString::fromLocal8Bit(stream.msg));

    compressedSize = 0;
    crc = crc32(0L, Z_NULL, 0);


    qint64 compressed;
    int flush = Z_NO_FLUSH;
    int zret = Z_OK;

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

        stream.next_in = (Bytef*) inBuffer;
        stream.avail_in = (uInt)read;

        // Tell zlib if this is the last chunk we want to encode
        // by setting the flush parameter to Z_FINISH
        flush = ((qint64)uncompressedSize == toRead) ? Z_FINISH : Z_NO_FLUSH;

        // Run deflate() on input until output buffer not full
        // finish compression if all of source has been read in
        do
        {
            stream.next_out = (Bytef*) outBuffer;
            stream.avail_out = DEFAULT_BUFSIZE;

            zret = deflate(&stream, flush);
            Q_ASSERT(zret != Z_STREAM_ERROR);

            // Write compressed data to file and empty buffer
            compressed = DEFAULT_BUFSIZE - stream.avail_out;

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

        } while (stream.avail_out == 0);

        // All input must be used
        Q_ASSERT(stream.avail_in == 0);

    } while (flush != Z_FINISH);

    Q_ASSERT(zret == Z_STREAM_END);
    end();

    return true;
}


bool DeflateCompressor::end()
{
    if (deflateEnd(&stream) != Z_OK)
        return setError(ZlibEndFailed, QString::fromLocal8Bit(stream.msg));

    return true;
}


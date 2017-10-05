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

#include "LzmaCompressor.h"

#include <QLibrary>
#include <zlib.h>

// read/write buffer size
#define LZMA_BUFSIZE  409600
//#define COMPRESSION_EXTREME true

extern "C" void *lz_alloc(void *, size_t , size_t size)
{
    void *p = new char [size];
    return p;
}

extern "C" void lz_free(void *, void *ptr)
{
    delete [] (char*)ptr;
}


LzmaCompressor::LzmaCompressor( int compressionLevel, QIODevice* inDev, QIODevice* outDev ) :
    Compressor(inDev, outDev)
{

    init( compressionLevel );
}


bool LzmaCompressor::init( int level )
{
    if (level < 1 || level > 9)
        level = 7;

    memset(&stream, 0, sizeof(stream));

//    lzma_allocator al;
//    al.alloc = lz_alloc;
//    al.free = lz_free;
    //stream.allocator = &al;

    lzma_options_lzma opt_lzma;
    if (lzma_lzma_preset(&opt_lzma, LZMA_PRESET_DEFAULT))
        return false;
    lzma_filter filters[] = {
        //{ LZMA_FILTER_X86, NULL },
        { LZMA_FILTER_LZMA2, &opt_lzma },
        { LZMA_VLI_UNKNOWN,  NULL }
    };

    // Initialize the encoder using the custom filter chain.
    lzma_ret ret = lzma_stream_encoder(&stream, filters, LZMA_CHECK_CRC64);

    if (ret != LZMA_OK)
        return false;


    // initialize xz encoder
//    int ret = lzma_easy_encoder (&stream, level >= 8 ? level | LZMA_PRESET_EXTREME : level, LZMA_CHECK_CRC64);
//    if (ret != LZMA_OK)
//        return false;

    // Create the read/write buffers
    inBuffer = new char[LZMA_BUFSIZE];
    outBuffer = new char[LZMA_BUFSIZE];

    return true;
}

bool LzmaCompressor::compressData( Encryption* encrypt )
{

    qint64 read = 0;
    qint64 written = 0;

    qint64 toRead = inDevice->size();
    lzma_action flush = LZMA_RUN;

    if ( !inBuffer )
        return setError(ZlibInitFailed, "BZIP2 Compression Init failed!");

    compressedSize = 0;
    crc = crc32(0L, Z_NULL, 0);

    qint64 compressed;
    int zret = LZMA_OK;

    do
    {
        read = inDevice->read(inBuffer, LZMA_BUFSIZE);
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

        stream.next_in = (uint8_t*)inBuffer;
        stream.avail_in = read;
        stream.next_out = (uint8_t*)outBuffer;
        stream.avail_out = LZMA_BUFSIZE;

        flush = ((qint64)uncompressedSize == toRead) ? LZMA_FINISH : LZMA_RUN;


        do
        {
            stream.next_out = (uint8_t*)outBuffer;
            stream.avail_out = LZMA_BUFSIZE;

            zret = lzma_code (&stream, flush);
            switch (zret)
            {
            case LZMA_MEM_ERROR:
                end();
                return setError(ZlibError, "LZMA error: Memory allocation failed");

            case LZMA_DATA_ERROR:
                end();
                return setError(ZlibError, "LZMA error: File size limits exceeded");
            }


            // Write compressed data to file and empty buffer
            compressed = LZMA_BUFSIZE - stream.avail_out;

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



    } while (zret != LZMA_STREAM_END );

    end();

    return true;
}


bool LzmaCompressor::end()
{
    lzma_end (&stream);
    return true;
}

#endif

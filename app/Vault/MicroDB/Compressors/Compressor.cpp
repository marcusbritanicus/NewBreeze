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

#include "Compressor.h"
#include "InflateCompressor.h"
#include "DeflateCompressor.h"
#include "Bzip2Compressor.h"
#include "Bzip2Decompressor.h"
#include "LzmaCompressor.h"
#include "LzmaDecompressor.h"
#include "StoreCompressor.h"



Compressor::Compressor(QIODevice* inDev, QIODevice* outDev) :
    inDevice(inDev),
    outDevice(outDev)
{
    error = 0;
    crc = 0;
    compressedSize = 0;
    uncompressedSize = 0;
    isAscii = false;

    inBuffer = outBuffer = NULL;
}

Compressor::~Compressor()
{
    if ( inBuffer )
        delete[] inBuffer;
    if ( outBuffer )
        delete[] outBuffer;
}


Compressor* Compressor::createCompressor( quint32 type, int level, QIODevice* inDev, QIODevice* outDev )
{
    Compressor* comp = 0;

    switch( type )
    {
    case methodStore:
        comp = new StoreCompressor( inDev, outDev );
        break;
#ifdef USE_BZIP2
    case methodBzip2:
        comp = new Bzip2Compressor( level, inDev, outDev );
        break;
#endif
#ifdef USE_LZMA
    case methodLzma:
        comp = new LzmaCompressor( level, inDev, outDev );
        break;
#endif

    case methodDeflate:
        comp  = new DeflateCompressor( level, inDev, outDev );
        break;
    }

    return comp;
}


Compressor* Compressor::createDecompressor( quint32 type, QIODevice* inDev, QIODevice* outDev )
{
    Compressor* comp = 0;

    switch( type )
    {
    case methodStore:
        comp = new StoreCompressor( inDev, outDev );
        break;
#ifdef USE_BZIP2
    case methodBzip2:
        comp = new Bzip2Decompressor( inDev, outDev );
        break;
#endif
#ifdef USE_LZMA
    case methodLzma:
        comp = new LzmaDecompressor( inDev, outDev );
        break;
#endif

    case methodDeflate:
        comp  = new InflateCompressor( inDev, outDev );
        break;
    }

    return comp;
}


void Compressor::checkIfBinary( char* buffer, quint32 size )
{
    // If the buffer contains a null character, then assume it's binary
    if ( !isAscii && !memchr( buffer, 0, size ) )
        isAscii = true;
}

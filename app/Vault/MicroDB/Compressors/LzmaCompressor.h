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

#ifndef LZMACOMPRESSOR_H
#define LZMACOMPRESSOR_H

#ifdef USE_LZMA

#include "Compressor.h"

#define LZMA_API_STATIC
#include <lzma.h>

class LzmaCompressor : public Compressor
{
public:
    LzmaCompressor(int compressionLevel, QIODevice* inDev, QIODevice* outDev = 0);

    virtual bool compressData( Encryption* = 0 );


private:
    virtual bool init( int compressionLevel );
    virtual bool end( );

    lzma_stream  stream;

};

#endif

#endif // LzmaCompressor

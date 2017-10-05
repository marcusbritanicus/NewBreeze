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

#ifndef BZIP2COMPRESSOR_H
#define BZIP2COMPRESSOR_H

#ifdef USE_BZIP2

#include "Compressor.h"

#include <bzlib.h>

class Bzip2Compressor : public Compressor
{
public:
    Bzip2Compressor(int compressionLevel, QIODevice* inDev, QIODevice* outDev = 0);

    virtual bool compressData( Encryption* = 0 );


private:
    virtual bool init( int compressionLevel );
    virtual bool end( );

    bz_stream bstream;          /* bzLib stream structure for bziped */
};

#endif

#endif // BZIP2COMPRESSOR_H

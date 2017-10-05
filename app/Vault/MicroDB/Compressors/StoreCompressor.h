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

#ifndef STORECOMPRESSOR_H
#define STORECOMPRESSOR_H

#include "Compressor.h"

class StoreCompressor : public Compressor
{
public:
    StoreCompressor( QIODevice* inDev, QIODevice* outDev = 0 );

    bool copyData( qint64 toRead );
    bool copyDataInSameFile( qint64 readPos, qint64 writePos, qint64 toRead );

    virtual bool compressData( Encryption* encrypt = 0);
    virtual bool decompressData( qint64 size, Encryption* encrypt = 0);

private:
};

#endif // STORECOMPRESSOR_H

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

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "encryption.h"

#include <QIODevice>

#define DEFAULT_BUFSIZE (16384)

/// Compressor types enum:
enum tMethod
{
    methodStore = 0,
    methodDeflate = 8
#ifdef USE_BZIP2
    ,methodBzip2 = 12
#endif
#ifdef USE_LZMA
    , methodLzma = 14
#endif
};


class Compressor
{
public:
    explicit Compressor(QIODevice* inDev, QIODevice* outDev);
    virtual ~Compressor();

    enum ErrorCode
    {
        Ok,
        ZlibInitFailed,
        ZlibEndFailed,
        ZlibError,
        ReadFailed,
        WriteFailed,
        UnknownError
    };

    static Compressor* createCompressor( quint32 Type, int level, QIODevice* inDev, QIODevice* outDev = 0 );
    static Compressor* createDecompressor( quint32 Type, QIODevice* inDev, QIODevice* outDev = 0 );


    virtual bool compressData( Encryption* = 0){ return false; }
    virtual bool decompressData( qint64, Encryption* = 0){ return false; }


    bool setError( int err, QString msg = QString() ){ error = err; errorMessage = msg; return false;}
    QString errorString(  ){ return errorMessage; }

    quint32 crc;
    quint64 compressedSize;
    quint64 uncompressedSize;

    void checkIfBinary( char* , quint32 );
    bool isAscii;

protected:


    QIODevice* inDevice;
    QIODevice* outDevice;

    char  *inBuffer;         // Buffer for reading data
    char  *outBuffer;        // Buffer for writing data

    int error;
    QString errorMessage;
};

#endif // COMPRESSOR_H

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

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QObject>
#include <QIODevice>

#define RAND_SEED 3141592654UL     // use PI as default pattern
#define AES_METHOD          (99)

/// Compressor types enum:
enum tEncryptionType
{
    encNone,
    encCRC = 8, // Z_DEFLATE
    encAES = AES_METHOD
};


class Encryption
{
public:
    Encryption();
    virtual ~Encryption(){}

    static Encryption* createEncryptor( quint32 type );

    virtual bool init( const QString& password ) = 0;
    virtual void encryptBytes( char* buffer, qint64 read) = 0;
    virtual void decryptBytes( char* buffer, qint64 read) = 0;

    virtual bool decryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc ) = 0;
    virtual qint32 encryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc) = 0;

    // Not 'pure virtual' as not all encryption methods will need this
    virtual qint32 decryptFooter(QIODevice* , quint16 ){ return 0;}
    virtual qint32 encryptFooter(QIODevice* ){ return 0;}


    virtual qint32 size(){ return 0; }

    bool isSupported( int method );
};

#endif // ENCRYPTION_H

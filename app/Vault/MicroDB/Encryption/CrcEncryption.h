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

#ifndef CRCENCRYPTION_H
#define CRCENCRYPTION_H

#include "encryption.h"

#define CRC_ENCRYPT_HEADER_SIZE 12

class CrcEncryption : public Encryption
{
public:
    CrcEncryption();

    virtual bool init( const QString& password );

    virtual void encryptBytes( char* buffer, qint64 read);
    virtual void decryptBytes( char* buffer, qint64 read);

    virtual bool decryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc );
    virtual qint32 encryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc );

    virtual qint32 size(){ return CRC_ENCRYPT_HEADER_SIZE; }

private:
    void initKeys( const QString& password );

    int decryptByte( );
    quint32 updateChecksum(const quint32& crc, const quint32& val);
    void updateKeys(int c);

    const quint32* crcTable;
    quint32 keys[3];
};


#endif // CRCENCRYPTION_H

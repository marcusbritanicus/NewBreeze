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

#include "CrcEncryption.h"
#include "iobuffer.h"
#include "Utils.h"

#include <QTime>

#include <zlib.h>

CrcEncryption::CrcEncryption()
{
    crcTable = (quint32*) get_crc_table();

}

bool CrcEncryption::init( const QString& password )
{
    initKeys( password );

    return true;
}

int CrcEncryption::decryptByte( )
{
    quint16 temp = ((quint16)(keys[2]) & 0xffff) | 2;
    return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
}


// Initializes decryption keys using a password
void CrcEncryption::initKeys( const QString& password )
{
    // Encryption keys initialization constants are taken from the
    // PKZip file format specification docs
    keys[0] = 305419896L;
    keys[1] = 591751049L;
    keys[2] = 878082192L;

    QByteArray pwdBytes = password.toLatin1();

    for (int i = 0; i < pwdBytes.size(); ++i)
        updateKeys( (int)pwdBytes[i]);
}

quint32 CrcEncryption::updateChecksum(const quint32& crc, const quint32& val)
{
    return quint32(crcTable[quint32(crc^val) & 0xff] ^ quint32(crc >> 8));
}

// Updates encryption keys.
void CrcEncryption::updateKeys( int c )
{
    keys[0] = updateChecksum(keys[0], c);
    keys[1] += keys[0] & 0xff;
    keys[1] = keys[1] * 134775813L + 1;
    keys[2] = updateChecksum(keys[2], ((int)keys[1]) >> 24);
}

// Encrypts a byte array.
void CrcEncryption::encryptBytes( char* buffer, qint64 read)
{
    char t;

    for (int i = 0; i < (int)read; ++i)
    {
        t = buffer[i];
        buffer[i] ^= decryptByte();
        updateKeys(t);
    }
}



// Decrypts an array of bytes
void CrcEncryption::decryptBytes( char* buffer, qint64 read )
{
    for (int i = 0; i < (int)read; ++i)
        updateKeys( buffer[i] ^= decryptByte());
}


bool CrcEncryption::decryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc )
{
    // read encryption keys
    char buffer[CRC_ENCRYPT_HEADER_SIZE];

    // Read the encryption header
    if ( ioDevice->read( buffer, CRC_ENCRYPT_HEADER_SIZE ) != CRC_ENCRYPT_HEADER_SIZE )
        return false;

    initKeys(password);

    // decrypt encryption header
    decryptBytes( buffer, CRC_ENCRYPT_HEADER_SIZE );

    return (buffer[CRC_ENCRYPT_HEADER_SIZE-1] == (char)HIBYTE( crc ) );
}


qint32 CrcEncryption::encryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc )
{
    char buffer[CRC_ENCRYPT_HEADER_SIZE];

    qsrand( QTime::currentTime().msec()^ RAND_SEED );

    int randByte;
    initKeys( password );
    for (int i = 0; i < CRC_ENCRYPT_HEADER_SIZE-2; ++i)
    {
        randByte = (qrand() >> 7) & 0xff;
        buffer[i] = decryptByte() ^ randByte;
        updateKeys( randByte );
    }

    // Encrypt encryption header
    initKeys(password);
    buffer[CRC_ENCRYPT_HEADER_SIZE-2] = (char)LOBYTE(crc);
    buffer[CRC_ENCRYPT_HEADER_SIZE-1] = (char)HIBYTE(crc);

    encryptBytes( buffer, CRC_ENCRYPT_HEADER_SIZE );

    return ioDevice->write(buffer, CRC_ENCRYPT_HEADER_SIZE);
}


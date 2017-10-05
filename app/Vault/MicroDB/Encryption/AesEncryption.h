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
/*
 ---------------------------------------------------------------------------
 Copyright (c) 1998-2008, Brian Gladman, Worcester, UK. All rights reserved.

 LICENSE TERMS

 The redistribution and use of this software (with or without changes)
 is allowed without the payment of fees or royalties provided that:

  1. source code distributions include the above copyright notice, this
     list of conditions and the following disclaimer;

  2. binary distributions include the above copyright notice, this list
     of conditions and the following disclaimer in their documentation;

  3. the name of the copyright holder is not used to endorse products
     built using this software without specific written permission.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 */



#ifndef AESENCRYPTION_H
#define AESENCRYPTION_H

#ifdef USE_AES

#include "encryption.h"

#define CRC_ENCRYPT_HEADER_SIZE 12

#define AES_PWVERIFYSIZE    (2)
#define AES_AUTHCODESIZE    (10)
#define AES_MAXSALTLENGTH   (16)
#define AES_VERSION         (0x0001)
#define AES_ENCRYPTIONMODE  (0x03)
#define AES_HEADERSIZE      (11)
#define AES_KEYSIZE(mode)   (64 + (mode * 64))

#include <aes.h>
#include <fileenc.h>
#include <prng.h>
#include <entropy.h>

class AesEncryption : public Encryption
{
public:
    AesEncryption( );
    virtual ~AesEncryption();

    virtual bool init( const QString& password );

    virtual void encryptBytes( char* buffer, qint64 read);
    virtual void decryptBytes( char* buffer, qint64 read);

    virtual bool decryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc );
    virtual qint32 encryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc );

    virtual qint32 decryptFooter(QIODevice* ioDevice, quint16 crc );
    virtual qint32 encryptFooter(QIODevice* ioDevice );

    virtual qint32 size(){ return headerSize; }



private:
    qint32 headerSize;

    unsigned char passverify[AES_PWVERIFYSIZE];
    unsigned char saltvalue[AES_MAXSALTLENGTH];
    quint32       saltlength;


    fcrypt_ctx aes_ctx;
    prng_ctx aes_rng[1];
};

#endif // USE_AES

#endif // CRCENCRYPTION_H

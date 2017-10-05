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

#ifdef USE_AES

#include "AesEncryption.h"

AesEncryption::AesEncryption(  )
{
    headerSize = 0;
    saltlength = 0;
}

AesEncryption::~AesEncryption()
{
}

bool AesEncryption::init( const QString& password )
{
    saltlength = SALT_LENGTH(AES_ENCRYPTIONMODE);

    prng_init(entropy_fun, aes_rng);
    prng_rand(saltvalue, saltlength, aes_rng);
    prng_end(aes_rng);

    fcrypt_init(AES_ENCRYPTIONMODE, (unsigned char*)password.toLatin1().data(), password.size(), saltvalue, passverify, &aes_ctx);

    headerSize = saltlength + AES_PWVERIFYSIZE + AES_AUTHCODESIZE;

    return true;
}


// Decrypts an array of bytes
void AesEncryption::decryptBytes( char* inBuffer, qint64 inBufferSize )
{
    if ( inBufferSize <= 0)
        return;

    fcrypt_decrypt( (unsigned char*)inBuffer, (unsigned int)inBufferSize, &aes_ctx);
}

// Encrypts a byte array
void AesEncryption::encryptBytes( char* inBuffer, qint64 inBufferSize )
{
    if ( inBufferSize <= 0)
        return;

    fcrypt_encrypt( (unsigned char*)inBuffer, (unsigned int)inBufferSize, &aes_ctx);
}



bool AesEncryption::decryptHeader(QIODevice* ioDevice, const QString& password, quint16 strength )
{
    if ((int)strength < 1 || strength > 3)
        return false;

    saltlength = SALT_LENGTH(strength);

    if ( ioDevice->read( (char*)saltvalue, saltlength ) != saltlength )
        return false;

    unsigned char readVerify[AES_PWVERIFYSIZE];
    if ( ioDevice->read( (char*)readVerify, AES_PWVERIFYSIZE ) != AES_PWVERIFYSIZE )
        return false;

    fcrypt_init((int)strength, (unsigned char*)password.toLatin1().data(), password.size(), saltvalue, passverify, &aes_ctx);

    // Validate the verify code (hence the password is valid)
    if (memcmp(readVerify, passverify, AES_PWVERIFYSIZE) != 0)
        return false;


    headerSize = saltlength + AES_PWVERIFYSIZE + AES_AUTHCODESIZE;

    return true;
}


qint32 AesEncryption::decryptFooter(QIODevice* ioDevice, quint16 check )
{
    Q_UNUSED(check);

    unsigned char authcode[AES_AUTHCODESIZE];
    unsigned char rauthcode[AES_AUTHCODESIZE];

    if ( ioDevice->read( (char*)authcode, AES_AUTHCODESIZE ) != AES_AUTHCODESIZE )
        return 0;

    if (fcrypt_end(rauthcode, &aes_ctx) != AES_AUTHCODESIZE)
        return 0;

    if (memcmp(authcode, rauthcode, AES_AUTHCODESIZE) != 0)
        return -1;

    return 1;
}

qint32 AesEncryption::encryptHeader(QIODevice* ioDevice, const QString& password, quint16 crc )
{
    Q_UNUSED( crc );
    init( password );

    ioDevice->write((char*)saltvalue, saltlength);

    return ioDevice->write((char*)passverify, AES_PWVERIFYSIZE);
}

qint32 AesEncryption::encryptFooter(QIODevice* ioDevice )
{
    unsigned char authcode[AES_AUTHCODESIZE];

    fcrypt_end(authcode, &aes_ctx);

    return ioDevice->write( (char*)authcode, AES_AUTHCODESIZE);
}

#endif


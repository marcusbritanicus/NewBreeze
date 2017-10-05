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

#include "encryption.h"
#include "CrcEncryption.h"
#include "AesEncryption.h"

Encryption::Encryption()
{

}

bool Encryption::isSupported( int method )
{
    return (method == encCRC
#ifdef USE_AES
            || method == encAES
#endif
            ) ? true : false;
}


Encryption* Encryption::createEncryptor( quint32 method  )
{
    Encryption* enc = 0;

    switch( method )
    {
    case encCRC:
        enc = new CrcEncryption(  );
        break;

#ifdef USE_AES
    case encAES:
        enc  = new AesEncryption(  );
        break;
#endif
    }

    return enc;
}

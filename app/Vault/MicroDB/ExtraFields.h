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

#ifndef EXTRAFIELDS_H
#define EXTRAFIELDS_H

#include <QMap>

#define ZIP_EXTRA_ZIP64             0x0001     // zip64
#define ZIP_EXTRA_WINZIP_AES        0x9901     // WinZip AES
#define ZIP_EXTRA_NTFS              0x000A     // NTFS file dates
#define ZIP_EXTRA_UNIX              0x000D     // UNIX file dates
#define ZIP_EXTRA_TIMESTAMP         0x5455     // extended timestamp
#define ZIP_EXTRA_UNICODE_PATH      0x7075     // Info-ZIP Unicode Path Extra Field
#define ZIP_EXTRA_UNICODE_COMMENT   0x6375     // Info-ZIP Unicode Comment Extra Field


class ExtraFields
{
public:
    ExtraFields();
    ExtraFields(const QByteArray& fields);
    virtual ~ExtraFields(){}

    virtual ExtraFields& operator=( const ExtraFields& );

    bool read( const QByteArray& fields );
    QByteArray write( );
    bool add( quint16 headerID, const QByteArray& data );

    // Some of the extra fields we are interseted in
    bool zip64( qint64* uncompressSize, qint64* compressSize = 0, qint64* offset = 0);
    void setZip64( qint64 uncompressSize, qint64 compressSize = 0, qint64 offset = 0 );
    bool ntfs( qint64* mtime = 0, qint64* atime = 0, qint64* ctime = 0);
    void setNtfs( qint64 mtime, qint64 atime, qint64 ctime );
    bool timestamp( qint64* mtime, qint64* atime = 0, qint64* ctime = 0);
    void setTimeStamp( qint64 mtime, qint64 atime = 0, qint64 ctime = 0);
    bool getUnix( qint64* mtime, qint64* atime = 0);
    void setUnix( qint64 mtime, qint64 atime );
    bool getWinZipAES( quint16& method, quint8& strength );
    void setWinZipAES( quint16 method );

    bool unicode( QString& name, quint16 headerID );

protected:
    QMap<quint16, QByteArray> mapFields;
};

#endif // EXTRAFIELDS_H

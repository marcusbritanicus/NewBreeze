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

#ifndef LOCALFILEHEADER_H
#define LOCALFILEHEADER_H

#include "zipglobal.h"
#include "ExtraFields.h"
#include <ZipFileInfo.h>

#include <QObject>
#include <QIODevice>
#include <QDateTime>
#include <QFileInfo>

#define VERSION_MADE_BY  MAKESHORT(20, ZipUtils::getOsMadeBy() ) // Version 2.0 and sys compat 0 for now
#define VERSION_NEEDED  20               // version 2.0 (default)
#define VERSION_NEEDED_ZIP64    45


#define LOCAL_FILE_HEADER_SIZE	30
#define LOCAL_FILE_HEADER_SIGNATURE     0x04034b50

#define DATA_DESCRIPTOR_SIZE        16
#define DATA_DESCRIPTOR_SIGNATURE       0x08074b50

#define ZIP_IS_ENCRYPTED            0x01    // bit 0
#define ZIP_IS_STRONG_ENCRYPTION    0x40    // bit 6
#define ZIP_WRITE_DATA_DESCRIPTOR   0x08    // bit 3
#define ZIP_IS_UTF8                 0x0800  // Bit 11

class CentralDirFileHeader;

class LocalFileHeader : public QObject
{
    Q_OBJECT
public:
    explicit LocalFileHeader(QObject *parent = 0);
    LocalFileHeader(const LocalFileHeader&);

    LocalFileHeader& operator=(const LocalFileHeader& other);

    virtual ~LocalFileHeader();


    void init( CentralDirFileHeader* );

    void setFileName( const QString& filename );

    virtual void initFromFile( const QFileInfo& file );
    virtual bool validate( LocalFileHeader* header);

    virtual bool read(QIODevice* ioDevice);
    void readExtraData(QIODevice* ioDevice);
    virtual qint64 write(QIODevice* ioDevice );
    void checkForExtraData( );

    virtual quint32 size() { return LOCAL_FILE_HEADER_SIZE + fileName.size() + extraField.size(); }
    virtual void setSizes( qint64 uncompSize, qint64 compSize, qint64 offset);

    virtual quint64 getCompressedSize();
    virtual quint64 getUncompressedSize();

    virtual ZipFileInfo getFileInfo();

    void setWinZipAES( );
    bool getWinZipAES( quint16& method, quint8& strength );

    qint64 writeDataDescriptor( QIODevice* ioDevice );
    void readDataDescriptor( QIODevice* ioDevice );

    bool isEncrypted()const { return generalFlag & ZIP_IS_ENCRYPTED; }          // Bit 1
    bool isStrongEncryption()const { return isEncrypted() && (generalFlag & ZIP_IS_STRONG_ENCRYPTION); }          // Bit 6
    bool hasDataDescriptor() const { return generalFlag & ZIP_WRITE_DATA_DESCRIPTOR; }   // Bit 3
    bool isUTF8()const { return generalFlag & ZIP_IS_UTF8; }          // Bit 11
    virtual bool isDirectory( );

    QDateTime lastModDate();

    virtual bool isValid()
    {
        return true;
    }

    bool isNewer( LocalFileHeader* header );

    quint32 signature;
    quint16 versionNeeded;
    quint16 generalFlag;
    quint16 compressionMethod;
    quint16 lastModFileTime;
    quint16 lastModFileDate;
    quint32 crc_32;
    quint32 compressedSize;
    quint32 uncompressedSize;
    quint16 filenameLength;
    quint16 extraFieldLength;
    QByteArray fileName;
    QByteArray extraField;

    ExtraFields* extraFields;
 };


#endif // LOCALFILEHEADER_H

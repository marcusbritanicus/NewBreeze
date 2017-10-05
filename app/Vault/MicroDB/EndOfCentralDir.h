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

#ifndef ENDOFCENTRALDIR_H
#define ENDOFCENTRALDIR_H

#include <QObject>
#include <QIODevice>

#define CENTRAL_DIR_END_SIZE	22
#define CENTRAL_DIR_END64_LOCATOR_SIZE	20
#define CENTRAL_DIR_END64_SIZE	56

#define CENTRAL_DIR_END_SIGNATURE    0x06054b50
#define CENTRAL_DIR_END64_SIGNATURE    0x06064b50
#define CENTRAL_DIR_END64_LOCATOR_SIGNATURE 0x07064b50

class EndOfCentralDir
{

public:
    EndOfCentralDir();

    void reset( );

    bool read(QIODevice* ioDevice);
    qint32 write(QIODevice* ioDevice);

    bool needsZip64() const
    {
        return numberOfThisDisk >= 0xffff || numberOfDiskCdStarts >= 0xffff ||
                numberOfCdEntriesOnDisk >= 0xffff || numberOfEntries >= 0xffff ||
                sizeOfCD >= 0xffffffff || offsetToStartOfCD >= 0xffffffff;
    }

    QString getComment(  );
    void setComment( const QString& cmt );

    bool isValid()
    {
        return  (signature == CENTRAL_DIR_END_SIGNATURE &&
                ((sizeOfCD || !numberOfEntries) && (numberOfEntries || !sizeOfCD) ));
    }

    qint32 size(){ return CENTRAL_DIR_END_SIZE + comment.size(); }

    quint32 signature;      // 0x06054b50
    quint16 numberOfThisDisk;      // assume no splits so = 0
    quint16 numberOfDiskCdStarts;    // = 0
    quint16 numberOfCdEntriesOnDisk;
    quint16 numberOfEntries;
    quint32 sizeOfCD;           // size of the central directory
    quint32 offsetToStartOfCD;  // relative to start of file
    quint16 commentLength;
    QByteArray comment;
};


class EndOfCentralDir64
{

public:
    EndOfCentralDir64();

    bool read(QIODevice* ioDevice);

    qint32 write(QIODevice* ioDevice);

    bool isValid()
    {
        return ( signature == CENTRAL_DIR_END64_SIGNATURE &&
                (numberOfEntries != numberOfCdEntriesOnDisk ||
                numberOfDiskCdStarts != 0 || numberOfThisDisk != 0 ||
                ((sizeOfCD || !numberOfEntries) && (numberOfEntries || !sizeOfCD))) );
    }

    qint32 size(){ return CENTRAL_DIR_END64_SIZE; }

    quint32 signature;      // 0x06064b50
    quint64 sizeOfzip64EndOfCDRecord;
    quint16 versionMadeBy;
    quint16 versionNeeded;
    quint32 numberOfThisDisk;      // assume no splits so = 0
    quint32 numberOfDiskCdStarts;    // = 0
    quint64 numberOfCdEntriesOnDisk;
    quint64 numberOfEntries;
    quint64 sizeOfCD;           // size of the central directory
    quint64 offsetToStartOfCD;  // relative to start of file
    QByteArray extensibleData;
};


#endif // ENDOFCENTRALDIR_H

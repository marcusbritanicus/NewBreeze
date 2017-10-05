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

#ifndef ZIPCENTRALDIR_H
#define ZIPCENTRALDIR_H

#include "EndOfCentralDir.h"
#include "CentralDirFileHeader.h"
#include "AbZip.h"
#include "ZipFileInfo.h"

#include <QList>
#include <QIODevice>


class ZipCentralDir
{

public:
    ZipCentralDir( );
    ~ZipCentralDir(  );

public: //protected:

    CentralDirFileHeader* findFile( const QString& file, AbZip::ZipOptions options = AbZip::None );
    bool read( QIODevice* ioDevice, AbZipPrivate* zip );
    qint64 write(QIODevice* ioDevice );

    void reset();

    void setModified(){ modified = true; }
    bool isModified(){ return modified; }

    void setOffsetToStartOfCD( qint64 pos );
    qint64 offsetToStartOfCD();

    qint64 numberOfEntries();
    void setNumberOfEntries( qint64 num );

    qint64 sizeOfCD();
    void setSizeOfCD( qint64 size );

    // Adjusts relativeOffsets for when we delete a file
    void adjustOffsets( qint64 fromOffset, qint64 size );

    ZipInfo getInfo();

//protected:

    qint64 locateSignature( quint32 signature, QIODevice* ioDevice  );
    qint64 bytesBeforeZip;          // for SFX stuff
    void calcBytesBeforeZip();

    QList<CentralDirFileHeader*> entries;

    qint64 startOfEndOfCD;
    EndOfCentralDir endOfCentralDir;        // We always have one of these
    EndOfCentralDir64* endOfCentralDir64;   // We may have one of these
    bool modified;
    bool isValid;

    // Iterator functions
    QMap<QString, CentralDirFileHeader*> sortedEntries;
    QMapIterator<QString, CentralDirFileHeader*> iteratorEntries;
    qint64 createSortedHeader();

    CentralDirFileHeader* getFirstFile();
    CentralDirFileHeader* getNextFile();
    bool hasNextFile();
};

#endif // ZIPCENTRALDIR_H

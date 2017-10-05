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

#ifndef ABZIP_P_H
#define ABZIP_P_H

#include "ZipCentralDir.h"


class AbZipPrivate
{
    friend class ZipArcive;
    friend class ZipCentralDir;

public:
    AbZipPrivate();
    AbZipPrivate(const QString& _zipName);
    AbZipPrivate(QIODevice *_ioDevice);

    virtual ~AbZipPrivate();

    bool open( AbZip::Mode mode, AbZip::ZipOptions options = AbZip::None );
    void close();
    void flushAndClose();
    bool isOpen()const;
    bool checkAutoOpenWrite();
    bool checkAutoOpenRead();

    bool backupOrDeleteCurrentZip( AbZip::ZipOptions options );

    bool addFile(const QString& file, const QString& root = QString(), AbZip::ZipOptions options = AbZip::AddRelativePaths, int level = -1 );
    bool addFile(const QFileInfo& srcFileInfo, const QString& arcFile, AbZip::ZipOptions options, int level = -1);

    bool addDirectory(const QString& srcPath, const QString& root = QString(), AbZip::ZipOptions options = AbZip::AddRelativePaths, int level = -1 );

    quint32 getBestCompressionMethod( QFile& srcFileInfo, AbZip::ZipOptions options );

    bool extractFile(const QString& filename, const QString& dirname, AbZip::ZipOptions options = AbZip::ExtractPaths);
    bool extractFile( CentralDirFileHeader* header, const QString& destPath, AbZip::ZipOptions options = AbZip::ExtractPaths);
    bool extractAll( AbZip& zip, const QString& destPath, const QString& fromRoot, AbZip::ZipOptions options);


    bool deleteFile(AbZip& zip, const QString& filename, AbZip::ZipOptions options = AbZip::None);
    bool deleteFile(CentralDirFileHeader* header, bool dontLogIt = false );

    bool renameFile(const QString& oldFilename, const QString& newFilename, AbZip::ZipOptions options);

    QList<ZipFileInfo> findFile( AbZip& zip, const QString& filename, const QString& root, AbZip::ZipOptions options = AbZip::Recursive );

    bool checkIntegrity();
    bool repairArchive();
    bool rebuildCentralDirectory( );

    bool setError( int err, QString msg = QString() );

    ZipInfo getInfo();

    void init();

    // The archive file name.
    QString zipName;

    // Command stats
    qint32 filesAdded;
    qint32 filesExtracted;
    qint32 filesDeleted;
    qint32 errorCount;


    // The io device to access the archive
    QIODevice *ioDevice;

    QString password;

    QStringList nameFilters;

    int errorCode;
    QString errorMsg;

    ZipCentralDir* centralDir;

    bool commitDeletedFiles();
    bool writeToTempFile( QIODevice* );

    QList<CentralDirFileHeader*> deletedEntries;

    QStringList storeTheseTypes;
};

#endif // ABZIP_P_H

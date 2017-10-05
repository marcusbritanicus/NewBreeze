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

#ifndef CENTRALDIRFILEHEADER_H
#define CENTRALDIRFILEHEADER_H

#include "zipglobal.h"
#include "LocalFileHeader.h"

#include <QObject>
#include <QIODevice>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>

#define CD_FILE_HEADER_SIZE         46
#define CD_FILE_HEADER_SIGNATURE    0x02014b50



class CentralDirFileHeader : public LocalFileHeader
{
    Q_OBJECT
public:
    explicit CentralDirFileHeader(QObject *parent = 0);
    CentralDirFileHeader(const CentralDirFileHeader & );
    virtual ~CentralDirFileHeader();

    CentralDirFileHeader& operator=(const CentralDirFileHeader& other);

    virtual void initFromFile( const QFileInfo& file );
    void initFromLocalHeader( const LocalFileHeader& localHeader ); // Used for recovery of bad CD


    virtual bool read(QIODevice* ioDevice);
    virtual qint64 write(QIODevice* ioDevice);

    void setPermissions( QFile::Permissions perm, bool isDir);
    QFile::Permissions  getPermissions();

    virtual void setSizes( qint64 uncompSize, qint64 compSize, qint64 offset);

    virtual quint64 getRelativeOffset();

    virtual bool isDirectory( );
    bool isValid()
    {
        return signature == CD_FILE_HEADER_SIGNATURE;
    }

    virtual quint32 size() { return CD_FILE_HEADER_SIZE + fileName.size() + extraField.size() + comment.size(); }

    virtual ZipFileInfo getFileInfo();

    void setFileName( const QString& filename );

    quint16 versionMadeBy;
    quint16 fileCommentLength;
    quint16 diskNumberStart;
    quint16 internalFileAttr;
    quint32 externalFileAttr;
    quint32 relativeOffset;
    QByteArray comment;

    // Additional members
    QString sFileName;                  // A QString version of the QByteArray filename - just for convienience
};


#endif // CENTRALDIRFILEHEADER_H

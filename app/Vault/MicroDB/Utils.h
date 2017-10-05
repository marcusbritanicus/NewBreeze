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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDateTime>

namespace ZipUtils {

#define MAKESHORT(low, high) ((quint16)((((quint16)(high)) << 8) | ((quint8)(low))))
#define LOBYTE(w) ((quint8)(w))
#define HIBYTE(w) ((quint8)(((quint16)(w) >> 8) & 0xFF))


/// just extract the filename part only from a full path
QString getFileName( const QString& strFullFile);

QString getFileSuffix( const QString& strFullFile);

QString stripDriveLetter( const QString& name );

/// Convert all path serarators into /
QString fixPathSeparators( const QString& strFullFile );


bool setFileDateTime( const QString& sFile, const QDateTime& time );

quint32 getFileAttributes( const QString& filePath );

quint8 getOsMadeBy();
}

#endif // UTILS_H

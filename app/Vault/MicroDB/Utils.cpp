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

#include <QFile>
#include <QFileInfo>

#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QSettings>
#include <QDir>

#include "Utils.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#include "utime.h"
#endif

namespace ZipUtils {


// Usefull function to just extract the file name from a full path
QString getFileName( const QString& strFullFile)
{
    QString strFile( fixPathSeparators( strFullFile ) );         // This will convert windows '\\' into '/'
    int index = strFile.lastIndexOf( '/' );
    if ( index >= 0 )
        return strFile.mid( index+1 );

    return strFile;
}

/// just extract the filename extension part only from a full path
QString getFileSuffix( const QString& name)
{
    int index = name.lastIndexOf( QLatin1Char('.') );
    if ( index > 0 )
        return name.mid( index );

    return "";
}

QString fixPathSeparators( const QString& strFullFile )
{
    QString strFile( strFullFile );         // This will convert windows '\' into char /
    if ( strFile.indexOf('\\') >= 0 )
        strFile.replace('\\', '/' );

    return strFile;
}

// Strip drive letter for network path and clean dots
QString stripDriveLetter( const QString& path )
{
    QString name = fixPathSeparators( path );         // This will convert windows '\\' into '/'
    while ( name.startsWith('.'))
        name = name.mid(1);
    int index = name.indexOf( ':' );
    if ( index >= 0 && name.size() > 2 )
        return name.mid( index+2 );   // e.g.  c:/abc/def  ->  abc/def

    if ( name.startsWith( '/') )
        return name.mid( 1 );   // e.g.  /abc/def  ->  abc/def

    return name;
}



#ifdef Q_OS_WIN

#define HILONG(value) ((quint32)((value) >> 32))
#define LOLONG(value) ((quint32)((value) & 0xffffffff))

// number of seconds from 1 Jan. 1601 00:00 to 1 Jan 1970 00:00 UTC
#define EPOCH_DIFF 11644473600LL

bool qDateTimeToFileTime(const QDateTime& date, FILETIME& ft )
{
    uint nSecSince1970 = date.toTime_t();

    quint64 result = EPOCH_DIFF + nSecSince1970;
    result *= 10000000ll;   // convert from Secs to 100 nano secs

    ft.dwHighDateTime = HILONG( result );
    ft.dwLowDateTime = LOLONG( result );

    return true;
}
#endif

bool setFileDateTime( const QString& sFile, const QDateTime& dateTime )
{
    bool f = false;
#ifdef Q_OS_WIN

    HANDLE hFile = ::CreateFileW( reinterpret_cast<const wchar_t *>(sFile.utf16()), GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        FILETIME ftCreate;

        qDateTimeToFileTime( dateTime, ftCreate );
        f = (bool)SetFileTime((HANDLE)hFile, &ftCreate, &ftCreate, &ftCreate);

        FlushFileBuffers( hFile);
        CloseHandle(hFile);
    }
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX)

    struct utimbuf t_buffer;
    t_buffer.actime = t_buffer.modtime = dateTime.toTime_t();
    return utime(sFile.toLocal8Bit().constData(), &t_buffer) == 0;

#endif
    return f;
}


quint32 getFileAttributes( const QString& filePath )
{
    quint32 attr = 0;
#ifdef Q_OS_WIN
    attr = GetFileAttributesW( reinterpret_cast<const wchar_t *>(filePath.utf16()) );
#else
    Q_UNUSED(filePath);
#endif
    return attr;
}



quint8 getOsMadeBy()
{
#if defined(Q_OS_IOS)
    return 7;   // not sure if this is right!
#elif defined(Q_OS_MAC)
    return 7;
//#elif defined(Q_OS_WIN)   // i'm gona assume DOS for Windows!
//    return 10;
#elif defined(Q_OS_LINUX)
    return 3;               // Not sure here either!
#elif defined(Q_OS_UNIX)
    return 3;
#else
    return 0;
#endif
}

}

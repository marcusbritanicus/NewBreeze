/*
    *
    * Global.hpp - Globally used header
    *
*/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <iostream>
#include <fstream>

// Qt Headers
#include <QtGui>
#include <QtCore>
#include <QtDBus>
#include <QtNetwork>

// C++ Standard Library
#include <fstream>

// C Standard Library
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// SystemWide Headers
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <utime.h>
#include <errno.h>
#include <sys/vfs.h>
#include <libgen.h>
#include <magic.h>
#include <sys/inotify.h>
#include <limits.h>

#include <MD5.hpp>
#include <NBSettings.hpp>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
	#include <QMimeDatabase>
	#include <QMimeType>
#else
	#include <NBMimeDatabase.hpp>
	#include <NBMimeType.hpp>
#endif

namespace NBIOMode {
	enum Mode {
		Copy,
		Move
	};
};

static QMimeDatabase mimeDb;

// MimeType and Progams mapping cache location
const QString mimeProgsCache( QDir( QDesktopServices::storageLocation( QDesktopServices::CacheLocation ) ).filePath( "NewBreeze/NBMimeProgs-unstable.cache" ) );

// Thumbnails Storage Directory
const QString thumbsDir( QDir( QDesktopServices::storageLocation( QDesktopServices::CacheLocation ) ).filePath( "NewBreeze/Thumbs-unstable/" ) );

// Suffix and Icon mapping cache location
const QString suffixIconPath( QDir( QDesktopServices::storageLocation( QDesktopServices::CacheLocation ) ).filePath( "NewBreeze/NBSuffixIcon-unstable.cache" ) );

// Suffix-Icon Map
static QMap<QString, QStringList> suffixIconMap;

// NewBreeze Preferences
static QSettings settings( "NewBreeze", "NewBreeze-unstable" );
static QSettings actionSettings( "NewBreeze", "CustomActions-unstable" );
static QSettings bookmarkSettings( "NewBreeze", "Bookmarks-unstable" );
static QSettings shortcutSettings( "NewBreeze", "Shortcuts-unstable" );
static QSettings thumbsInfo( thumbsDir + "Thumbs.cache", QSettings::NativeFormat );

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

extern NBSettings Settings;

#endif

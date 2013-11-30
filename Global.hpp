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
#include <QtNetwork>
#include <QCryptographicHash>

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
		ACopy,
		Copy,
		Move
	};
};

namespace NBInfoMode {
	enum Mode {
		Application,
		FileSystem,
		Catalog
	};
};

static QMimeDatabase mimeDb;

// MimeType and Progams mapping cache location
const QString mimeProgsCache( QDir( QDesktopServices::storageLocation( QDesktopServices::CacheLocation ) ).filePath( "NewBreeze/NBMimeProgs.cache" ) );

// Thumbnails Storage Directory
const QString thumbsDir( QDir( QDesktopServices::storageLocation( QDesktopServices::CacheLocation ) ).filePath( "NewBreeze/Thumbs/" ) );

// NewBreeze Preferences
static QSettings actionSettings( "NewBreeze", "CustomActions" );
static QSettings bookmarkSettings( "NewBreeze", "Bookmarks" );
static QSettings thumbsInfo( thumbsDir + "Thumbs.cache", QSettings::NativeFormat );

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

static NBSettings *Settings = NBSettings::instance();

#endif

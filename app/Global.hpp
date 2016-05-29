/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtNetwork>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "common.hpp"
#include "NBSettings.hpp"

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

// Thumbnails Storage Directory
const QString thumbsDir( QDir( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation ) ).filePath( "NewBreeze/Thumbs/" ) );

// NewBreeze Preferences
static QSettings actionSettings( "NewBreeze", "CustomActions" );
static QSettings bookmarkSettings( "NewBreeze", "Bookmarks" );
static QSettings thumbsInfo( thumbsDir + "Thumbs.cache", QSettings::NativeFormat );

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

static NBSettings *Settings = NBSettings::instance();

static QString logPath = QDir( QStandardPaths::writableLocation( QStandardPaths::GenericConfigLocation ) ).filePath( "NewBreeze/newbreeze.log" );
static FILE *nblog = fopen( logPath.toLocal8Bit().data(), "a" );

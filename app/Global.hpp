/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include <QtNetwork>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "newbreeze.hpp"
#include "NBSettings.hpp"

static QMimeDatabase mimeDb;

// Thumbnails Storage Directory
const QString thumbsDir( QDir( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation ) ).filePath( "NewBreeze/Thumbs/" ) );

// NewBreeze Preferences
static QSettings actionSettings( "NewBreeze", "CustomActions" );
static QSettings bookmarkSettings( "NewBreeze", "Bookmarks" );
static QSettings thumbsInfo( thumbsDir + "Thumbs.cache", QSettings::NativeFormat );

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

extern NBSettings *Settings;
extern QHash<QString, QStringList> fileInfoHash;

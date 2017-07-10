/*
	*
	* NBIconManager.hpp - Icon Manager for NewBreeze
	*
*/

#pragma once

#include "Global.hpp"

// #include "newbreeze.hpp"
// #include <QtGui>

// static QMimeDatabase mimeDb;

// /* Thumbnails Storage Directory */
// const QString thumbsDir( QDir( QStandardPaths::writableLocation( QStandardPaths::GenericCacheLocation ) ).filePath( "NewBreeze/Thumbs/" ) );

// /* NewBreeze Preferences */
// static QSettings actionSettings( "NewBreeze", "CustomActions" );
// static QSettings bookmarkSettings( "NewBreeze", "Bookmarks" );
// static QSettings thumbsInfo( thumbsDir + "Thumbs.cache", QSettings::NativeFormat );

typedef QMap<QString, QString> DeviceInfo;
typedef QPair<QString, QString> BookMark;

class NBIconManager {

	public:
		/* Unique single instance */
		static NBIconManager *instance();

		/* Icon string for file type: @mName: Mime Name */
		QStringList iconsForFile( QString mName, QString file = QString() );

		/* Icon string for file type: @mName: Mime Name */
		QStringList icon( QString iName );

		/* Icon string for file type: @mName: Icon Name */
		bool hasIcon( QString iName );

		/* Thumbnail for an image file */
		QString imageThumbnail( QString );

	private:
		/* Private init function */
		NBIconManager();

		/* Generate theme database */
		void generateThemeDatabase();

		/* Global Instance */
		static NBIconManager *iMgr;
		bool init;

		/* Default theme location */
		QString iconThemePath;
};

class NBIconProvider {

	/* This is where our theme is set from the settings */
	static QString nbIconTheme;

	public :
		/* Return the icon string given the path and mime type */
		static QString icon( QString path, QMimeType mime = QMimeType() );

		/* Check if we have thumbnail corresponding to @path and @hashPath */
		static bool thumb( QString path, QString hashPath );

		/* Check if the theme has a icon named as the given parameter */
		static bool hasIcon( QString );

		/* Check if the theme has a icon named as the first parameter in theme second parameter */
		static QString themeIcon( QString, QString theme = QString() );

		/* Icon string from /usr/share/pixmaps */
		static QString pixmapIcon( QString );

	private :
		/* Do we have a stored thumb named @param1 with hash @param2 */
		static bool hasStoredThumb( QString, QString );

		/* Theme diretory listing */
		static QStringList getThemeDirs( QString );

		/* Custom icon from the inbuilt cache */
		static QString getCustomIcon( QString );

		/* Save a image thumbnail */
		static bool saveThumb( QString, QString );
};

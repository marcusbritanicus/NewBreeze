 /*
	*
	* NBIconManager.hpp - Icon Manager for NewBreeze
	*
*/

#pragma once

#include "Global.hpp"

class NBIconManager {

	public:
		/* Unique single instance */
		NBIconManager *instance();

		/* Icon string for file */
		QString iconStringForFile( QString, QMimeType mType = QMimeType() );

		/* Icon string for file */
		QByteArray imageThumbnail( QString, QMimeType mType = QMimeType() );

	private:
		/* Private init function */
		NBIconManager();

		/* Function to populate themeDirsHash */
		void readThemeDirectories();

		/* Function to populate themeFallbackHash */
		void readThemeInheritance();

		/* Global Instance */
		static NBIconManager *iMgr;
		bool init;

		/* We store the themes and their directories here */
		QHash<QString, QStringList> themeDirsHash;

		/* Store the fallbacks per theme, non-recursive */
		QHash<QString, QStringList> themeFallbackHash;

		/* Default theme location */
		QString iconThemePath;

		/* Create a thumbnail database object */
		QSqlDatabase thumbsDb;

		/* Create an icon database object */
		QSqlDatabase iconsDb;
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

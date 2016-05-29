/*
	*
	* NBIcon.hpp - NBIcon.cpp header
	*
*/

#pragma once

#include <Global.hpp>

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

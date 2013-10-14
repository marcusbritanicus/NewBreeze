/*
	*
	* NBIcon.hpp - NBIcon.cpp header
	*
*/

#ifndef NBICONPROVIDER_HPP
#define NBICONPROVIDER_HPP

#include <Global.hpp>

class NBIconProvider {

	/* This is where our theme is set from the settings */
	static QString nbIconTheme;

	public :
		/* Return the icon string given the path and mime type */
		static QString icon( QString path, QMimeType mime = QMimeType() );
		static bool thumb( QString path, QString hashPath );

	private :
		/* Check if the theme has a icon named as the given parameter */
		static bool hasIcon( QString );

		/* Do we have a stored thumb named @param1 with hash @param2 */
		static bool hasStoredThumb( QString, QString );

		/* Theme diretory listing */
		static QStringList getThemeDirs( QString );

		/* Custom icon from the inbuilt cache */
		static QString getCustomIcon( QString );

		/* Save a image thumbnail */
		static bool saveThumb( QString, QString );
};

#endif

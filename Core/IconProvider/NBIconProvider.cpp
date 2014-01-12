/*
	*
	* NBIcon.cpp - Icon class for NewBreeze
	*
*/

#include <NBIconProvider.hpp>
#include <NBSystemInfo.hpp>
#include <NBTools.hpp>

/*
	*
	* Convenience functions:
	* 		getInheritedThemes
	* 		getFallbackThemeDirs
	*
*/

// Initialize nbIconTheme to current settings value
QString NBIconProvider::nbIconTheme = NBSystemIconTheme();

inline QStringList getInheritedThemes( QString theme ) {

	// Inherited themes list
	QStringList inherited;
	inherited << QSettings( QDir( "/usr/share/icons/" + theme ).filePath( "index.theme" ), QSettings::IniFormat ).value( "Icon Theme/Inherits" ).toStringList();

	return inherited;
};

inline QStringList getFallbackThemeDirs( QString systemTheme ) {
	// If @p systemTheme is not the System Icon Theme returned by @f NBSystemTheme() then you may not get any icons at all

	QStringList fallbackDirs;

	// Fallback Theme
	QSettings fallbackSett( QDir( "/usr/share/icons/" + systemTheme ).filePath( "index.theme" ), QSettings::IniFormat );
	fallbackDirs << fallbackSett.value( "Icon Theme/Directories" ).toStringList();

	// Inherited fallback themes
	if ( fallbackSett.value( "Icon Theme/Inherits" ).toStringList().count() ) {
		foreach( QString inherited, fallbackSett.value( "Icon Theme/Inherits" ).toStringList() ) {
			QSettings fiSett( QDir( "/usr/share/icons/" ).filePath( inherited + "/index.theme" ), QSettings::IniFormat );
			fallbackDirs << fiSett.value( "Icon Theme/Directories" ).toStringList();
		}
	}

	return fallbackDirs;
}

/*
	*
	* NBIconProvider class functions
	*
*/

QString NBIconProvider::icon( QString path, QMimeType mimetype ) {

	if ( not mimetype.isValid() )
		mimetype = mimeDb.mimeTypeForFile( path );

	QString mimeIcon = mimetype.iconName();
	QString genericIcon = mimetype.genericIconName();

	// If the path is a directory
	if ( mimetype.name() == QString( "inode/directory" ) ) {
		QString icoStr = QSettings( QDir( path ).filePath( ".directory" ), QSettings::NativeFormat ).value( "Desktop Entry/Icon" ).toString();
		if ( !icoStr.isNull() and ( hasIcon( icoStr ) or exists( icoStr ) ) ) {
			/* This means we have a file named @v icoStr or a theme icon named @v icoStr */
			return icoStr;
		}

		else {
			if ( hasIcon( "folder" ) )
				/* FIXME: getIconPath( "folder" ) */
				return QString( "folder" );

			else if ( hasIcon( mimeIcon ) )
				return mimeIcon;

			else if ( hasIcon( genericIcon ) )
				return genericIcon;

			else
				return QString( ":/icons/folder.png" );
		}
	}

	// If it is a desktop file
	else if ( mimetype.name() == QString( "application/x-desktop" ) ) {
		// I won't use @v mimeIcon from mimetype, 'coz the icons for
		// application/x-executable is always application-x-executable
		QSettings dSettings( path, QSettings::NativeFormat );
		QString icoStr = dSettings.value( "Desktop Entry/Icon" ).toString();

		if ( hasIcon( icoStr ) )
			return QString( icoStr );

		else if ( hasIcon( "application-x-desktop" ) )
			return QString( "application-x-desktop" );

		else if ( hasIcon( "application-x-executable" ) )
			return QString( "application-x-executable" );

		else
			return QString( ":/icons/exec.png" );
	}

	// If it is an executable
	else if ( mimetype.name() == QString( "application/x-executable" ) ) {
		// I won't use @v mimeIcon from mimetype, 'coz the icons for
		// application/x-executable is always application-x-executable

		if ( hasIcon( "application-x-executable" ) )
			return QString( "application-x-executable" );

		else
			return QString( ":/icons/exec.png" );
	}

	// If it is a shared-object or a dynamic link library
	else if ( mimetype.name() == QString( "application/x-sharedlib" ) ) {
		// I think the icon I have is more stylish than the oxygen icon, or any other :P
		// if ( hasIcon( mimeIcon ) )
			// return QIcon::fromTheme( getIconPath( mimeIcon ) );

		// else
			return QString( ":/icons/binary.png" );
	}

	else {
		// If it is a image (save thumbnail) and image previews are set
		if ( ( mimetype.name().startsWith( "image" ) or mimetype.name().startsWith( "video/mng" ) ) and ( Settings->General.ImagePreviews ) ) {
			if ( mimetype.name().contains( "djvu" ) ) {
				if ( hasIcon( "image-vnd.djvu" ) )
					return "image-vnd.djvu";

				else if ( hasIcon ( "application-x-office-document" ) )
					return "application-x-office-document";

				else
					return "image-x-generic";
			}

			QString hashPath = QDir( thumbsDir ).absoluteFilePath( MD5( path ) );
			if ( hasStoredThumb( path, hashPath ) )
				return hashPath;

			else
				return ( saveThumb( path, hashPath ) ? hashPath : QString( "image-x-generic" ) );
		}

		else if ( hasIcon( mimeIcon ) )
			return QString( mimeIcon );

		else if ( hasIcon( genericIcon ) )
			return QString( genericIcon );

		else
			return getCustomIcon( mimetype.name() );
	}

	return QString();
};

bool NBIconProvider::thumb( QString path, QString hashPath ) {

	return saveThumb( path, hashPath );
};

bool NBIconProvider::hasIcon( QString icon ) {

	/*
		*
		* List of themes
		*
		* First we input our stored theme.
		* Then get its inherited themes.
		* We check if our theme is system theme.
		* If yes, end of story
		* Else, add the system theme and
		*
	*/

	if ( exists( icon ) )
		return true;

	QStringList themes = QStringList() << NBSystemIconTheme();
	themes << getInheritedThemes( themes.at( 0 ) );
	themes << "hicolor";
	themes.removeDuplicates();

	// Folders
	QString iconDir = QString( "/usr/share/icons/" );
	QStringList iconDirs;
	foreach( QString theme, themes )
		iconDirs << getThemeDirs( QDir( iconDir ).filePath( theme ) );

	iconDirs << getFallbackThemeDirs( themes[ 0 ] );
	iconDirs.removeDuplicates();

	// Search in each of the directories if the icon is present
	foreach( QString theme, themes ) {
		foreach( QString str, iconDirs ) {
			if ( exists( iconDir + theme + "/" + str + "/" + icon + ".png" ) )
				return true;

			if ( exists( iconDir + theme + "/" + str + "/" + icon + ".svg" ) )
				return true;

			if ( exists( iconDir + theme + "/" + str + "/" + icon ) )
				return true;
		}
	}

	return false;
};

QString NBIconProvider::pixmapIcon( QString icon ) {

	if ( exists( QString( "/usr/share/pixmaps/%1.png" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1.png" ).arg( icon );

	else if ( exists( QString( "/usr/share/pixmaps/%1.xpm" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1.xpm" ).arg( icon );

	else if ( exists( QString( "/usr/share/pixmaps/%1" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1" ).arg( icon );

	else
		return QString();
};

bool NBIconProvider::hasStoredThumb( QString path, QString hashPath ) {

	if ( exists( hashPath ) )
		return ( thumbsInfo.value( path ).toDateTime() == QFileInfo( path ).lastModified() );

	return false;
};

QStringList NBIconProvider::getThemeDirs( QString themeDir ) {

	// Main theme directories
	QSettings iconSett( QDir( themeDir ).filePath( "index.theme" ), QSettings::IniFormat );
	QStringList iconDirs = iconSett.value( "Icon Theme/Directories" ).toStringList();

	return iconDirs;
};

QString NBIconProvider::getCustomIcon( QString mime ) {

	if ( mime.startsWith( "audio" ) )
		return QString ( ":/icons/audio.png" );

	else if ( mime.startsWith( "image" ) )
		return QString ( ":/icons/image.png" );

	else if ( mime.startsWith( "video" ) )
		return QString ( ":/icons/video.png" );

	else if ( isArchiveAlt( mime ) )
		return QString ( ":/icons/archive.png" );

	else if ( mime.startsWith( "text" ) )
		return QString ( ":/icons/text.png" );

	else if ( mime.startsWith( "application" ) )
		return QString( ":/icons/binary.png" );

	else
		return QString( ":/icons/unknown.png" );
};

bool NBIconProvider::saveThumb( QString path, QString hashPath ) {

	QImage thumb = QImage( path ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );
	if ( thumb.save( hashPath, "PNG", 0 ) ) {
		thumbsInfo.setValue( path, QFileInfo( path ).lastModified() );
		thumbsInfo.sync();
		return true;
	}

	return false;
};

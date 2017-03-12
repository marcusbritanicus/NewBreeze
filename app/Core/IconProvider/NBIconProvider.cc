/*
	*
	* NBIcon.cpp - Icon class for NewBreeze
	*
*/

#include "NBIconProvider.hh"

/*
	*
	* Convenience functions:
	* 		getInheritedThemes
	* 		getFallbackThemeDirs
	*
*/

// Initialize nbIconTheme to current settings value
QString NBIconProvider::nbIconTheme = Settings->General.IconTheme;

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

	/*
		*
		* For directories, we will have to search multiple locations. One inside the directory for .directory. However this will not apply for
		* folder encrypted using encfs. Their data will be stored in the parent directory's .dreictory file. The .directory inside the path
		* always takes precedence. Thus we search the path first, then the parent.
		*
	*/
	if ( mimetype.name() == QString( "inode/directory" ) ) {

		/* Path Settings */
		QSettings settPath( QDir( path ).filePath( ".directory" ), QSettings::NativeFormat );

		/* Icon from the Path Settings */
		QString icoStr = settPath.value( "Desktop Entry/Icon" ).toString();
		if ( !icoStr.isNull() and ( hasIcon( icoStr ) or exists( icoStr ) ) ) {
			/* This means we have a file named @v icoStr or a theme icon named @v icoStr */
			return icoStr;
		}

		/* Parent Settings object */
		QSettings settPrnt( QDir( dirName( path ) ).filePath( ".directory" ), QSettings::NativeFormat );

		/* Check if the current directory is an EncFS source */
		if ( settPrnt.allKeys().contains( "EncFS/" + baseName( path ) ) )
			return QString( ":/icons/folder-locked.png" );

		/* Other wise it might be EncFS target */
		Q_FOREACH( QString key, settPrnt.allKeys() ) {
			if ( settPrnt.value( key ).toString() == baseName( path ) ) {
				/* If it is mounted, we return folder-unlocked */
				Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allVirtualMounts() ) {
					if ( info.mountPoint() == path )
						return QString( ":/icons/folder-unlocked.png" );
				}

				/* Otherwise we return folder-locked */
				return QString( ":/icons/folder-locked.png" );
			}
		}

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
				if ( hasIcon ( "application-x-office-document" ) )
					return "application-x-office-document";

				else if ( hasIcon ( "x-office-document" ) )
					return "x-office-document";

				else if ( hasIcon( mimeIcon ) )
					return mimeIcon;

				else if ( hasIcon( genericIcon ) )
					return genericIcon;

				else
					return "image-x-generic";
			}

			QString hashPath = QDir( thumbsDir ).absoluteFilePath( MD5( path ) );
			if ( hasStoredThumb( path, hashPath ) )
				return hashPath;

			else
				return ( saveThumb( path, hashPath ) ? hashPath : QString( "image-x-generic" ) );
		}

		// Android Package
		if ( mimetype.name().contains( "android.package" ) ) {
			if ( hasIcon( "application-x-java-archive" ) )
				return "application-x-java-archive";

			else if ( hasIcon ( "application-zip" ) )
				return "application-zip";

			else
				return "package-x-generic";
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

	/* Inbuilt icon absolute path */
	if ( exists( icon ) )
		return true;

	/* Get NB IconTheme and System Icon Theme */
	QStringList themes = QStringList() << Settings->General.IconTheme << NBSystemIconTheme();

	/* Themes inherited by system theme */
	themes << getInheritedThemes( themes.at( 0 ) );
	themes << getInheritedThemes( themes.at( 1 ) );

	/* We always include this: generic */
	themes << "hicolor";

	/* Clear duplicates */
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

QString NBIconProvider::themeIcon( QString icon, QString theme ) {

	/* Inbuilt icon absolute path */
	if ( exists( icon ) )
		return icon;

	if ( not theme.count() )
		theme = Settings->General.IconTheme;

	/* Get NB IconTheme and System Icon Theme */
	QStringList themes = QStringList() << Settings->General.IconTheme << NBSystemIconTheme();

	/* Themes inherited by system theme */
	themes << getInheritedThemes( themes.at( 0 ) );
	themes << getInheritedThemes( themes.at( 1 ) );

	/* We always include this: generic */
	themes << "hicolor";

	/* Clear duplicates */
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
				return iconDir + theme + "/" + str + "/" + icon + ".png";

			if ( exists( iconDir + theme + "/" + str + "/" + icon + ".svg" ) )
				return iconDir + theme + "/" + str + "/" + icon + ".svg";

			if ( exists( iconDir + theme + "/" + str + "/" + icon ) )
				return iconDir + theme + "/" + str + "/" + icon;
		}
	}

	/* As a last resort, check if the icon exists in the pixmaps directory */
	return pixmapIcon( icon );
};

QString NBIconProvider::pixmapIcon( QString icon ) {

	if ( exists( QString( "/usr/share/pixmaps/%1.png" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1.png" ).arg( icon );

	else if ( exists( QString( "/usr/share/pixmaps/%1.xpm" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1.xpm" ).arg( icon );

	else if ( exists( QString( "/usr/share/pixmaps/%1" ).arg( icon ) ) )
		return QString( "/usr/share/pixmaps/%1" ).arg( icon );

	else if ( exists( icon ) )
		return icon;

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

	if ( not exists( path ) )
		return false;

	// Cheat scaling: http://blog.qt.io/blog/2009/01/26/creating-thumbnail-preview/
	QImage thumb = QImage( path ).scaled( 512, 512, Qt::KeepAspectRatio ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );

	if ( thumb.save( hashPath, "PNG", 0 ) ) {
		thumbsInfo.setValue( path, QFileInfo( path ).lastModified() );
		thumbsInfo.sync();
		return true;
	}

	return false;
};

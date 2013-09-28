/*
	*
	* NBThumbsCache.cpp - Thumbnail handler for NewBreeze
	*
*/

#include <NBThumbsCache.hpp>
#include <MD5.hpp>
#include <NBTools.hpp>

/*
	*
	* TODO
	*

	*
	* Make use of the system icon theme for icon rendering along with the user chosen theme.
	* This ensures maximum number of icon get rendered based on the mimetypes.
	*
	* Also avoid saving the suffixes from $PATH. All are executables and most don't have suffixes
	* In files like gcc-4.7, '7' is not a suffix. Hence don't store them
	*

	*
	* DefaultThemeDetection
	*     kde:		<QSettings> .kde/share/config/kdeglobals/[Icons]/Theme
	*     gnome:	$ gconftool --get /desktop/gnome/interface/icon_theme
	*     xfce:		$ xfconf -c xsettings -p /Net/IconTheme
	*     razorqt:	<QSettings> .razor/razor.conf/[General]/icon_theme
	*
*/

QPixmap NBThumbsCache::thumbnail( QString path, QSize iconSize ) {

	if ( hasStoredThumb( path ) )
		return getThumb( path ).scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

	else
		return storeThumb( path ).scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

	return QPixmap();
};

QPixmap NBThumbsCache::getThumb( QString path ) {

	QString hashPath = QDir( thumbsDir ).absoluteFilePath( md5( path.toStdString() ).c_str() );
	return QPixmap( hashPath );
};

QPixmap NBThumbsCache::storeThumb( QString path ) {

	QString hashPath = QDir( thumbsDir ).absoluteFilePath( md5( path.toStdString() ).c_str() );
	QPixmap thumb = QPixmap( path ).scaled( QSize( 128, 128 ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
	thumb.save( hashPath, "PNG", 50 );

	thumbsInfo.setValue( path, QFileInfo( path ).lastModified() );
	thumbsInfo.sync();

	return thumb;
};

bool NBThumbsCache::hasStoredThumb( QString path ) {

	if ( QFileInfo( QDir( thumbsDir ).absoluteFilePath( md5( path.toStdString() ).c_str() ) ).exists() )
		if ( thumbsInfo.value( path ).toDateTime() != QFileInfo( path ).lastModified() )
			return false;

		else
			return true;
	else
		return false;
};

QPixmap NBIcon::icon( QFileInfo ftype, QSize iconSize ) {

	/*
		*
		* We have a directory
		* Check if a custom icon is set. If yes return it.
		* Else return the theme icon QIcon::fromTheme( ... ).pixmap( ... )
		*
	*/
	if ( ftype.isDir() ) {
		QSettings dSett( QDir( ftype.absoluteFilePath() ).filePath( ".directory" ), QSettings::NativeFormat );
		QString icoStr = dSett.value( "Desktop Entry/Icon" ).toString();
		if ( !icoStr.isNull() ) {
			// NBDebugPartMsg( icoStr + " " );
			return QIcon::fromTheme( icoStr, QIcon( icoStr ) ).pixmap( iconSize );
		}

		else {
			// NBDebugPartMsg( "<theme icon> "  );
			return QIcon::fromTheme( "folder", QIcon::fromTheme( "inode-directory", QIcon( ":/icons/folder.png" ) ) ).pixmap( iconSize );
		}
	}

	/*
		*
		* So we have a file here
		*
	*/
	else if ( ftype.isFile() ) {

		/*
			*
			* If there is no suffix, example 'Makefile' or 'README', use a QPixmapCache to store the icon
			* In case of hidden files, as in '.bashrc' or '.directory' and the like, treat them as no-suffix-files
			* and handle them similarly.
			* Suffixes like .part or .crdownload or .cache are partial or temporary files, so save the icon in
			* QPixmapCache
			*
		*/
		QStringList restricted = QStringList() << "crdownload" << "cache" << "part";
		restricted << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "0";
		if ( ftype.suffix().isEmpty() or ftype.fileName().startsWith( "." ) or restricted.contains( ftype.suffix().toLower() ) )
			return NoSuffixIcon( ftype.absoluteFilePath() ).scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		/*
			*
			* Now we are sure to have a a non-hidden file with a suffix. Load it as usual.
			*   1. Chec if the user wants image previews. If yes, proceed to 2. Else skip to 3.
			*   2. Check if the file is an image file. If yes then return the image.
			*   3. Check if it is executable. If yes return application-x-executable
			*   4. Now we need the generic icons list. Get it. If we find the suffix, good.
			*      Else, try getting it from @getIconForNewSuffix(). If no icons are found,
			*      return an empty QPixmap.
			*      If we have a non-empty @gIconList recurse through it. See If it is from
			*      the program cache, return as QIcon( ... ).pixmap( ... ), otherwise we'll
			*      get the icon from the theme, QIcon::fromTheme( ... ).pixmap( ... )
			*
		*/

		// Images detection
		if ( Settings.General.ImagePreviews ) {
			QList<QByteArray> images = QImageReader::supportedImageFormats();
			if ( images.contains( ftype.suffix().toLower().toUtf8() ) ) {
				// NBDebugPartMsg( "image" );
				return NBThumbsCache::thumbnail( ftype.absoluteFilePath(), iconSize );
			}
		}

		// Executable detection
		// Mostly ftype.suffix().isEmpty() is not required.
		if ( ftype.isExecutable() and ftype.suffix().isEmpty() ) {
			// NBDebugPartMsg( "executable" );
			return QIcon::fromTheme( "application-x-executable", QIcon( ":/icons/exec.png" ) ).pixmap( iconSize );
		}

		// Generic icons
		else {
			QStringList gIconList = suffixIconMap.value( ftype.suffix().toLower() );
			if ( !gIconList.count() )
				gIconList = getIconForNewSuffix( ftype.absoluteFilePath() );

			if ( !gIconList.count() ) {
				// NBDebugPartMsg( "Not found" );
				return QPixmap();
			}

			foreach( QString gIconName, gIconList ) {
				if ( gIconName.startsWith( ":/icons" ) ) {
					if ( !QIcon( gIconName ).pixmap( iconSize ).isNull() )
						// NBDebugPartMsg( "local: " + gIconName + " " );

					return QPixmap( gIconName ).scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation );
				}

				if ( hasIcon( gIconName ) ) {
					if ( !QIcon::fromTheme( gIconName ).pixmap( iconSize ).isNull() )
						// NBDebugPartMsg( "fromTheme: " + gIconName + " " );

					return QIcon::fromTheme( gIconName ).pixmap( iconSize );
				}
			}

			// NBDebugPartMsg( "Not found" );
			return QPixmap();
		}
	}

	// This means we don't have a file, nor a directory. So what the hell is it?
	// Return an empty pixmap
	else {
		return QPixmap();
	}
};

QIcon NBIcon::icon( QString suffix, QString mimetype ) {

	if ( mimetype == QString( "inode/directory" ) ) {
		return QIcon::fromTheme( "folder", QIcon::fromTheme( "inode-directory", QIcon( ":/icons/folder.png" ) ) );
	}

	else if ( mimetype == QString( "application/x-executable" ) ) {
		return QIcon::fromTheme( "application-x-executable", QIcon( ":/icons/exec.png" ) );
	}

	else {
		QStringList gIconList = suffixIconMap.value( suffix.toLower() );
		if ( !gIconList.count() )
			gIconList = getIconForNewMime( mimetype );

		if ( !gIconList.count() )
			return QIcon();

		foreach( QString gIconName, gIconList ) {
			if ( gIconName.startsWith( ":/icons" ) )
				return QIcon( gIconName );

			if ( hasIcon( gIconName ) )
				return QIcon::fromTheme( gIconName );
		}
	}

	return QIcon();
};

void NBIcon::loadCacheToMemory() {

	QTime time = QTime::currentTime();
	time.start();

	buildCache();
	qDebug() << "Built the Suffix-Icon cache in" << time.elapsed() << "milliseconds";

	time.restart();
	QSettings suffixIconCache( suffixIconPath, QSettings::NativeFormat );
	QStringList suffixIconList = suffixIconCache.allKeys();

	foreach( QString key, suffixIconList ) {
		QStringList value = suffixIconCache.value( key ).toStringList();
		suffixIconMap.insert( key, value );
	}

	qDebug() << "Loaded the Suffix-Icon cache in" << time.elapsed() << "milliseconds";
};

bool NBIcon::hasIcon( QString icon ) {

	// The main theme
	QString iconDir = QDir( "/usr/share/icons/" ).filePath( QIcon::themeName() );
	QSettings iconSett( QDir( iconDir ).filePath( "index.theme" ), QSettings::IniFormat );

	// Main theme directories
	QStringList iconDirs = iconSett.value( "Icon Theme/Directories" ).toStringList();
	if ( !iconDirs.count() ) {
		return false;
	}

	// Inherited themes
	if ( iconSett.value( "Icon Theme/Inherits" ).toStringList().count() ) {
		foreach( QString inherited, iconSett.value( "Icon Theme/Inherits" ).toStringList() ) {
			QSettings iSett( QDir( "/usr/share/icons/" ).filePath( inherited + "/index.theme" ), QSettings::IniFormat );
			iconDirs << iSett.value( "Icon Theme/Directories" ).toStringList();
		}
	}

	// Search in each of the directories if
	foreach( QString str, iconDirs ) {
		if ( QFileInfo( iconDir + "/" + str + "/" + icon + ".png" ).exists() ) {
			// NBDebugMsg( "Found icon: " + iconDir + "/" + str + "/" + icon + ".png", true );
			return true;
		}

		if ( QFileInfo( iconDir + "/" + str + "/" + icon + ".svg" ).exists() ) {
			// NBDebugMsg( "Found icon: " + iconDir + "/" + str + "/" + icon + ".svg", true );
			return true;
		}
	}

	return false;
};

void NBIcon::buildCacheOld() {

	/*
		*
		* # ================= TO BE RUN IN A THREAD ================= #
		*
		* Parse the globs file. For each mimetype corresponding to a suffix, check if there is a icon in the theme :
		*     @mimetype type/subtype converted to icon becomes type-subtype.
		*     Its checked as NBIcon::hasIcon( type-subtype )
		* If the theme has the icon then it is saved, else a generic icon is loaded from generic-icons file
		*
	*/

	// Suffix Icon Map  - The final mapping that will be saved
	QMap<QString, QString> sfxIcnMap;

	// MimeType GenericIcon Map
	QMap<QString, QString> mimeIconMap;

	QFile gicons( "/usr/share/mime/generic-icons" );
	gicons.open( QFile::ReadOnly );
	QStringList giconsList = QString( gicons.readAll() ).split( "\n", QString::SkipEmptyParts );
	gicons.close();

	foreach( QString gicon, giconsList )
		mimeIconMap.insert( gicon.split( ":" )[ 0 ], gicon.split( ":" )[ 1 ] );

	QFile globs( "/usr/share/mime/globs" );
	globs.open( QFile::ReadOnly );
	QStringList globsList = QString( globs.readAll() ).split( "\n", QString::SkipEmptyParts );
	globs.close();

	foreach( QString glob, globsList ) {
		if ( glob.startsWith( "#" ) )
			continue;

		QString key = glob.split( ":" )[ 1 ].replace( "*.", "" );
		QString mime = glob.split( ":" )[ 0 ];
		QString micon = QString( mime ).replace( "/", "-" );
		QString generic = mimeIconMap.value( mime, QString() );

		if ( hasIcon( micon ) )
			sfxIcnMap.insert( key, micon );

		else if ( generic.isEmpty() or !hasIcon( generic ) ) {
			if ( mime.startsWith( "audio" ) )
				sfxIcnMap.insert( key, QString( ":/icons/audio.png" ) );

			else if ( mime.startsWith( "image" ) )
				sfxIcnMap.insert( key, QString( ":/icons/image.png" ) );

			else if ( mime.startsWith( "video" ) )
				sfxIcnMap.insert( key, QString( ":/icons/video.png" ) );

			else if ( isArchiveAlt( mime ) )
				sfxIcnMap.insert( key, QString( ":/icons/archive.png" ) );

			else if ( mime.startsWith( "text" ) )
				sfxIcnMap.insert( key, QString( ":/icons/text.png" ) );

			else if ( mime.startsWith( "application" ) and hasIcon( "application-octet-stream" ) )
				sfxIcnMap.insert( key, QString( "application-octet-stream" ) );
		}

		else
			sfxIcnMap.insert( key, QString( ":/icons/unknown.png" ) );
	}

	QFile cache( suffixIconPath );
	cache.open( QFile::WriteOnly );
	foreach( QString key, sfxIcnMap.keys() )
		cache.write( qPrintable( ( key + "\xff" + sfxIcnMap.value( key ) + "\n" ) ) );

	cache.close();
};

void NBIcon::buildCache() {

	/*
		*
		* # ================= TO BE RUN IN A THREAD ================= #
		*
		* Parse the globs file. For each mimetype corresponding to a suffix, check if there is a icon in the theme :
		*     @mimetype type/subtype converted to icon becomes type-subtype.
		*     Its checked as NBIcon::hasIcon( type-subtype )
		* If the theme has the icon then it is saved into a QStringList. Generic icon loaded from generic-icons file is also added.
		*
	*/

	// Suffix Icon Map  - The final mapping that will be saved
	QMap<QString, QStringList> sfxIcnMap;

	// MimeType GenericIcon Map
	QMap<QString, QString> mimeIconMap;

	QFile gicons( "/usr/share/mime/generic-icons" );
	gicons.open( QFile::ReadOnly );
	QStringList giconsList = QString( gicons.readAll() ).split( "\n", QString::SkipEmptyParts );
	gicons.close();

	foreach( QString gicon, giconsList )
		mimeIconMap.insert( gicon.split( ":" )[ 0 ], gicon.split( ":" )[ 1 ] );

	QFile globs( "/usr/share/mime/globs" );
	globs.open( QFile::ReadOnly );
	QStringList globsList = QString( globs.readAll() ).split( "\n", QString::SkipEmptyParts );
	globs.close();

	foreach( QString glob, globsList ) {
		if ( glob.startsWith( "#" ) )
			continue;

		QString key = glob.split( ":" )[ 1 ].replace( "*.", "" );
		QString mime = glob.split( ":" )[ 0 ];
		QString micon = QString( mime ).replace( "/", "-" );
		QString generic = mimeIconMap.value( mime );

		// Check if micon exists. If yes add it
		if ( hasIcon( micon ) ) {
			sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << micon );
		}

		// Check if generic exists. If yes add it. End of Story
		if ( hasIcon( generic ) ) {
			sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << generic );
		}

		// Otherwise get an icon from our inbuild cache for certain types only
		else if ( generic.isEmpty() or !hasIcon( generic ) ) {
			if ( mime.startsWith( "audio" ) )
				if ( hasIcon( "audio-x-generic" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "audio-x-generic" ) );

				else
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( ":/icons/audio.png" ) );

			else if ( mime.startsWith( "image" ) )
				if ( hasIcon( "image-x-generic" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "image-x-generic" ) );

				else
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( ":/icons/image.png" ) );

			else if ( mime.startsWith( "video" ) )
				if ( hasIcon( "video-x-generic" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "video-x-generic" ) );

				else
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( ":/icons/video.png" ) );

			else if ( isArchiveAlt( mime ) )
				if ( hasIcon( "package-x-generic" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "package-x-generic" ) );

				else
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( ":/icons/archive.png" ) );

			else if ( mime.startsWith( "text" ) )
				if ( hasIcon( "text-x-generic" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "text-x-generic" ) );

				else if ( hasIcon( "text-plain" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "text-plain" ) );

				else
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( ":/icons/text.png" ) );

			else if ( mime.startsWith( "application" ) and hasIcon( "application-octet-stream" ) )
					sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString ( "application-octet-stream" ) );
		}

		// Otherwise set it as unknown
		else {
			sfxIcnMap.insert( key, QStringList( sfxIcnMap.value( key ) ) << QString( ":/icons/unknown.png" ) );
		}
	}

	QSettings cache( suffixIconPath, QSettings::NativeFormat );
	foreach( QString key, sfxIcnMap.keys() )
		cache.setValue( key, QStringList( sfxIcnMap.value( key ) ) );
};

QStringList NBIcon::getIconForNewSuffix( QString path ) {

	// Obsolete code below
	/*
		*
		* if ( QFileInfo( path ).baseName().startsWith( "." ) )
		*		return QStringList();
		*
	*/

	QString key = QFileInfo( path ).suffix().toLower();
	QString mime = getMimeTypeAlt( path );
	if ( mime.isEmpty() )
		mime = getMimeType( path );

	QString micon = QString( mime ).replace( "/", "-" );
	QString generic = QString( mime ).section( "/", 0, 0 ) + QString( "-x-generic" );

	if ( !key.isEmpty() ) {
		QSettings cache( suffixIconPath, QSettings::NativeFormat );

		if ( hasIcon( micon ) ) {
			cache.setValue( key, cache.value( key ).toStringList() << micon );
			suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << micon );
		}

		if ( hasIcon( generic ) ) {
			cache.setValue( key, cache.value( key ).toStringList() << generic );
			suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << generic );
		}

		else if ( generic.isEmpty() or !hasIcon( generic ) ) {
			if ( mime.startsWith( "audio" ) )
				if ( hasIcon( "audio-x-generic" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "audio-x-generic" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "audio-x-generic" ) );
				}

				else {
					cache.setValue( key, cache.value( key ).toStringList() << ":/icons/audio.png" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( ":/icons/audio.png" ) );
				}

			else if ( mime.startsWith( "image" ) )
				if ( hasIcon( "image-x-generic" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "image-x-generic" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "image-x-generic" ) );
				}

				else {
					cache.setValue( key, cache.value( key ).toStringList() << ":/icons/image.png" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( ":/icons/image.png" ) );
				}

			else if ( mime.startsWith( "video" ) )
				if ( hasIcon( "video-x-generic" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "video-x-generic" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "video-x-generic" ) );
				}

				else {
					cache.setValue( key, cache.value( key ).toStringList() << ":/icons/video.png" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( ":/icons/video.png" ) );
				}

			else if ( isArchiveAlt( mime ) )
				if ( hasIcon( "package-x-generic" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "package-x-generic" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "package-x-generic" ) );
				}

				else {
					cache.setValue( key, cache.value( key ).toStringList() << ":/icons/archive.png" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( ":/icons/archive.png" ) );
				}

			else if ( mime.startsWith( "text" ) )
				if ( hasIcon( "text-x-generic" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "text-x-generic" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "text-x-generic" ) );
				}

				else if ( hasIcon( "text-plain" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "text-plain" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "text-plain" ) );
				}

				else {
					cache.setValue( key, cache.value( key ).toStringList() << ":/icons/text.png" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( ":/icons/text.png" ) );
				}

			else if ( mime.startsWith( "application" ) and hasIcon( "application-octet-stream" ) ) {
					cache.setValue( key, cache.value( key ).toStringList() << "application-octet-stream" );
					suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString ( "application-octet-stream" ) );
			}
		}

		else {
			cache.setValue( key, cache.value( key ).toStringList() << QString( ":/icons/unknown.png" ) );
			suffixIconMap.insert( key, QStringList( suffixIconMap.value( key ) ) << QString( ":/icons/unknown.png" ) );
		}
	}

	return suffixIconMap.value( key );
};

QStringList NBIcon::getIconForNewMime( QString mime ) {

	QString micon = QString( mime ).replace( "/", "-" );
	QString generic = QString( mime ).section( "/", 0, 0 ) + QString( "-x-generic" );

	QStringList iconList;

	if ( hasIcon( micon ) )
		iconList << micon;

	if ( hasIcon( generic ) )
		iconList << generic;

	else if ( generic.isEmpty() or !hasIcon( generic ) ) {
		if ( mime.startsWith( "audio" ) )
			iconList << QString ( ":/icons/audio.png" );

		else if ( mime.startsWith( "image" ) )
			iconList << QString ( ":/icons/image.png" );

		else if ( mime.startsWith( "video" ) )
			iconList << QString ( ":/icons/video.png" );

		else if ( isArchiveAlt( mime ) )
			iconList << QString ( ":/icons/archive.png" );

		else if ( mime.startsWith( "text" ) )
			iconList << QString ( ":/icons/text.png" );

		else if ( mime.startsWith( "application" ) and hasIcon( "application-octet-stream" ) )
			iconList << QString ( "application-octet-stream" );
	}

	else {
		iconList << QString( ":/icons/unknown.png" );
	}

	return iconList;
};

QPixmap NBIcon::NoSuffixIcon( QString path ) {

	QPixmap pix;

	if ( QPixmapCache::find( path, &pix ) )
		return pix;

	QString mime = getMimeTypeAlt( path );
	if ( mime.isEmpty() )
		mime = getMimeType( path );

	QString micon = QString( mime ).replace( "/", "-" );
	QString generic = QString( mime ).section( "/", 0, 0 ) + QString( "-x-generic" );

	if ( ( mime == QString( "application/x-executable" ) ) or ( mime == QString( "application/x-sharedlib" ) ) ) {
		pix = QIcon::fromTheme( "application-x-executable", QIcon( ":/icons/exec.png" ) ).pixmap( QSize( 48, 48 ) );
		QPixmapCache::insert( path, pix );
		return pix;
	}

	if ( hasIcon( micon ) ) {
		pix = QIcon::fromTheme( micon ).pixmap( QSize( 48, 48 ) );
		QPixmapCache::insert( path, pix );
		return pix;
	}

	if ( hasIcon( generic ) )
		pix = QIcon::fromTheme( generic ).pixmap( QSize( 48, 48 ) );

	else if ( generic.isEmpty() or !hasIcon( generic ) ) {
		if ( mime.startsWith( "audio" ) )
			if ( hasIcon( "audio-x-generic" ) )
				pix = QIcon::fromTheme( "audio-x-generic" ).pixmap( QSize( 48, 48 ) );

			else
				pix.load( ":/icons/audio.png" );

		else if ( mime.startsWith( "image" ) )
			if ( hasIcon( "image-x-generic" ) )
				pix = QIcon::fromTheme( "image-x-generic" ).pixmap( QSize( 48, 48 ) );

			else
				pix.load( ":/icons/image.png" );

		else if ( mime.startsWith( "video" ) )
			if ( hasIcon( "video-x-generic" ) )
				pix = QIcon::fromTheme( "video-x-generic" ).pixmap( QSize( 48, 48 ) );

			else
				pix.load( ":/icons/video.png" );

		else if ( isArchiveAlt( mime ) )
			if ( hasIcon( "package-x-generic" ) )
				pix = QIcon::fromTheme( "package-x-generic" ).pixmap( QSize( 48, 48 ) );

			else
				pix.load( ":/icons/archive.png" );

		else if ( mime.startsWith( "text" ) )
			if ( hasIcon( "text-x-generic" ) )
				pix = QIcon::fromTheme( "text-x-generic" ).pixmap( QSize( 48, 48 ) );

			else if ( hasIcon( "text-plain" ) )
				pix = QIcon::fromTheme( "text-plain" ).pixmap( QSize( 48, 48 ) );

			else
				pix.load( ":/icons/text.png" );

		else if ( mime.startsWith( "application" ) and hasIcon( "application-octet-stream" ) )
			pix = QIcon::fromTheme( "application-octet-stream" ).pixmap( QSize( 48, 48 ) );
	}

	else
		pix.load( QString( ":/icons/unknown.png" ) );

	QPixmapCache::insert( path, pix );
	return pix;
};

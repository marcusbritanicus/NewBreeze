/*
	*
	* NBIconManager.cpp - Icon Manager class for NewBreeze
	*
*/

/*
	*
	* The new Icon Manager replaces the old NBIconProvider. The functionalities will be enhanced greatly in this class.
	* New:
	* 		- Icons for images will be stored in a single massive database.
	* 		- Standard icon database will be created which matches a mimetype to icons paths of all sizes
	*				MimeType 1							[ Path 1, Path 2, Path 3, ... ]
	*		  This ensures that we do not have to search the same icon multiple times.
	* 		- We create this Standard Icon Database once using the startup wizard. Subsequent updation needs to be done manually.
	* 		- The database gets updated if the icon theme is changed.
	*
*/

#include "NBIconManager.hpp"

#if QT_VERSION >= 0x050000
	#include <QtConcurrent>
#endif

/* Theme Database */
static QSettings mdb( NBXdg::userDir( NBXdg::XDG_CACHE_HOME ) + "NewBreeze/mimetypes.db", QSettings::NativeFormat );
static QSettings idb( NBXdg::userDir( NBXdg::XDG_CACHE_HOME ) + "NewBreeze/icons.db", QSettings::NativeFormat );

static inline QStringList listFiles( QString path ) {

	struct dirent **fileList;
	int entries = scandir( path.toLocal8Bit().data(), &fileList, NULL, NULL );
	if ( entries > 0 ) {
		QStringList files;
		for( int i = 0; i < entries; i++ ) {
			/* Ignore . and .. */
			struct dirent *entry = fileList[ i ];
			if ( not strcmp( entry->d_name, "." ) or not strcmp( entry->d_name, ".." ) )
				continue;

			files << path + QString::fromLocal8Bit( entry->d_name );
		}

		return files;
	}

	return QStringList();
};

NBIconManager *NBIconManager::iMgr = NULL;

NBIconManager* NBIconManager::instance() {

	/* If the Plugin Manager has already been init, then return the instance */
	if ( iMgr and iMgr->init )
		return iMgr;

	/* Init our plugin manager */
	iMgr = new NBIconManager();

	/* If init succeeds, we set the init flag and return */
	iMgr->init = true;

	return iMgr;
};

NBIconManager::NBIconManager() {

	/* Set the default theme path */
	iconThemePath = QString( "/usr/share/icons/" );

	if ( not exists( mdb.fileName() ) or not exists( idb.fileName() ) )
		generateThemeDatabase();
};

QStringList NBIconManager::iconsForFile( QString mName, QString file ) {

	if ( mName.isEmpty() ) {
		if ( file.isEmpty() )
			return QStringList() << ":/icons/unknwon.png";

		mName = mimeDb.mimeTypeForFile( file ).name();
	}

	if ( not mName.compare( "inode/directory" ) ) {

		if ( not file.endsWith( "/" ) )
			file += "/";

		/* User defined directory icon from @path/.directory */
		QSettings settPath( QDir( file ).filePath( ".directory" ), QSettings::NativeFormat );
		QString icoStr = settPath.value( "Desktop Entry/Icon" ).toString();
		if ( !icoStr.isNull() and ( hasIcon( icoStr ) or exists( icoStr ) ) ) {
			/* This means we have a file named @v icoStr or a theme icon named @v icoStr */
			return QStringList() << icoStr;
		}

		/* EncFS Encrypted/Decrypted Folder */
		QSettings settPrnt( QDir( dirName( file ) ).filePath( ".directory" ), QSettings::NativeFormat );
		if ( settPrnt.allKeys().contains( "EncFS/" + baseName( file ) ) )
			return QStringList() << ":/icons/folder-locked.png";

		Q_FOREACH( QString key, settPrnt.allKeys() ) {
			if ( settPrnt.value( key ).toString() == baseName( file ) ) {
				/* If it is mounted, we return folder-unlocked */
				Q_FOREACH( NBDeviceInfo info, NBDeviceManager::allVirtualMounts() ) {
					if ( info.mountPoint() == file )
						return QStringList() << ":/icons/folder-unlocked.png";
				}

				/* Otherwise we return folder-locked */
				return QStringList() << ":/icons/folder-locked.png";
			}
		}
	}

	// If it is a desktop file
	else if ( mName == QString( "application/x-desktop" ) ) {

		QSettings dSettings( file, QSettings::NativeFormat );
		QString icoStr = dSettings.value( "Desktop Entry/Icon" ).toString();
		return ( hasIcon( icoStr ) ? QStringList() << icoStr : mdb.value( mName ).toStringList() );
	}

	/* This is an image file */
	else if ( mName.startsWith( "image/" ) ) {
		if ( Settings->General.ImagePreviews ) {
			/* DJVU issue comes only when we have to show previews */
			if ( mName.contains( "djv" ) )
				return mdb.value( mName ).toStringList();

			return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	return mdb.value( mName ).toStringList();
};

bool NBIconManager::hasIcon( QString iName ) {

	return idb.allKeys().contains( iName );
};

QStringList NBIconManager::icon( QString iName ) {

	return idb.value( iName, QStringList() << ":/icons/unknown.png" ).toStringList();
};

void NBIconManager::generateThemeDatabase() {

	/* List the theme inheritence */
	QStringList themes;
	themes << Settings->General.IconTheme;
	QSettings index( iconThemePath + themes[ 0 ] + "/index.theme", QSettings::NativeFormat );
	themes << index.value( "Icon Theme/Inherits" ).toStringList();

	/* Add hicolor to every theme and them remove duplicates */
	themes << "hicolor";
	themes.removeDuplicates();

	/* Get all theme directories */
	QStringList themeDirs;
	Q_FOREACH( QString theme, themes ) {
		QSettings index( iconThemePath + theme + "/index.theme", QSettings::NativeFormat );
		Q_FOREACH( QString dir, index.value( "Icon Theme/Directories" ).toStringList() )
			themeDirs << iconThemePath + theme + "/" + dir + "/";
	}

	int counter = 0;
	Q_FOREACH( QMimeType mType, mimeDb.allMimeTypes() ) {
		QString mIcon = mType.iconName();
		QString gmIcon = mType.genericIconName();

		QStringList paths;

		Q_FOREACH( QString dir, themeDirs ) {
			if ( not dir.contains( "mimetypes" ) )
				continue;

			if ( exists( dir + mIcon + ".png" ) )
				paths << dir + mIcon + ".png";

			if ( exists( dir + mIcon + ".svg" ) )
				paths << dir + mIcon + ".svg";

			if ( exists( dir + gmIcon + ".png" ) )
				paths << dir + gmIcon + ".png";

			if ( exists( dir + gmIcon + ".svg" ) )
				paths << dir + gmIcon + ".svg";
		}

		if ( paths.count() )
			mdb.setValue( mType.name(), paths );

		else
			mdb.setValue( mType.name(), QStringList() << ":/icons/unknown.png" );

		counter++;
	}

	/* Sync our database */
	mdb.sync();

	qDebug() << "Added" << counter << "mimetypes to the database";

	counter = 0;
	Q_FOREACH( QString dir, themeDirs ) {
		Q_FOREACH( QString file, listFiles( dir ) ) {
			if ( not file.endsWith( ".png" ) and not file.endsWith( ".svg" ) and not file.endsWith( ".xpm" ) and not file.endsWith( ".svgz" ) )
				continue;

			QString base = baseName( file ).replace( ".png", "" ).replace( ".svg", "" ).replace( ".xpm", "" ).replace( ".svgz", "" );
			QStringList paths = idb.value( base, QStringList() ).toStringList();
			paths << file;

			idb.setValue( base, paths );
			counter++;
		}
	}

	qDebug() << "Added" << counter << "icons to the database";

	/* DJVU Fix */
	if ( hasIcon( "application-x-office-document" ) )
		mdb.setValue( "image/vnd.djvu+multipage", QStringList() << "" << "" );

	/* Executable Fix */
	mdb.setValue( "application/x-executable", QStringList() << ":/icons/exec.png" );

	/* Shared Object Fix */
	mdb.setValue( "application/x-sharedlib", QStringList() << ":/icons/binary.png" );

	/* APK Fix */
	mdb.setValue( "application/vnd.android.package-archive", icon( "package-x-generic" ) );

	/* Desktop file Fix */
	mdb.setValue( "application/x-desktop", QStringList() << ":/icons/exec.png" );

	/* PPTX Fix */
	mdb.setValue( "application/vnd.openxmlformats-officedocument.presentationml.presentation", icon( "application-vnd.ms-powerpoint" ) );
};

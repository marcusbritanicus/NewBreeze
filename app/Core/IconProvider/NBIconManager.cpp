/*
	*
	* NBIconManager.cpp - Icon Manager class for NewBreeze
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

	/* Generate the theme database */
	generateThemeDatabase();
};

QStringList NBIconManager::iconsForFile( QString mName, QString file ) {

	if ( mName.isEmpty() ) {
		if ( file.isEmpty() )
			return QStringList() << ":/icons/unknwon.png";

		mName = mimeDb.mimeTypeForFile( file ).name();
	}

	/* This is an ODF file: if thumbnail exists, we return it otherwise a normal mime icons */
	QStringList odf;
	odf << "application/vnd.oasis.opendocument.text" << "application/vnd.oasis.opendocument.presentation";
	odf << "application/vnd.oasis.opendocument.spreadsheet" << "application/vnd.oasis.opendocument.graphics";

	/* If we have a directory */
	if ( not mName.compare( "inode/directory" ) ) {				// QString.compare(...) returns 0 for a match

		if ( not file.endsWith( "/" ) )
			file += "/";

		/* User defined directory icon from @path/.directory NewBreeze icons */
		QSettings settPath( QDir( file ).filePath( ".directory" ), QSettings::NativeFormat );
		QString icoStr = settPath.value( "NewBreeze/Icon" ).toString();
		if ( hasIcon( icoStr ) )
			return icon( icoStr );

		else if ( exists( icoStr ) )
			return QStringList() << icoStr;

		/* User defined directory icon from @path/.directory Global Icons */
		icoStr = settPath.value( "Desktop Entry/Icon" ).toString();
		if ( hasIcon( icoStr ) )
			return icon( icoStr );

		else if ( exists( icoStr ) )
			return QStringList() << icoStr;

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

		return mdb.value( "inode/directory" ).toStringList();
	}

	// If it is a desktop file
	else if ( mName == QString( "application/x-desktop" ) ) {

		QSettings dSettings( file, QSettings::NativeFormat );
		QString icoStr = dSettings.value( "Desktop Entry/Icon" ).toString();
		return ( hasIcon( icoStr ) ? QStringList() << icoStr : mdb.value( mName ).toStringList() );
	}

	/* This is an djvu file */
	else if ( mName.contains( "djv" ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/DjVuPreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
				return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	/* This is an image file */
	else if ( mName.startsWith( "image/" ) or not mName.compare( "video/mng" ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/ImagePreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
				return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	/* This is an video file */
	else if ( mName.startsWith( "video/" ) and mName.compare( "video/mng" ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/VideoPreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
				return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	/* This is a ODF file */
	else if ( odf.contains( mName ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/OdfPreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
				return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	/* This is a pdf file */
	else if ( mName.contains( "pdf" ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/PdfPreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
				return QStringList() << QDir( thumbsDir ).absoluteFilePath( MD5( file ) );
		}
	}

	/* This is a pdf file */
	else if ( mName.contains( "epub" ) ) {
		if ( Settings->value( "View/FilePreviews" ) and Settings->value( "View/ePubPreview" ) ) {
			if ( exists( QDir( thumbsDir ).absoluteFilePath( MD5( file ) ) ) )
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
	themes << ( QString )Settings->value( "View/IconTheme" );
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
	themeDirs.removeDuplicates();

	/* Clear our older icon database */
	idb.clear();

	/* Create our icon database */
	Q_FOREACH( QString dir, themeDirs ) {
		Q_FOREACH( QString file, listFiles( dir ) ) {
			if ( not ( file.endsWith( ".png" ) or file.endsWith( ".svg" ) or file.endsWith( ".xpm" ) or file.endsWith( ".svgz" ) ) )
				continue;

			QString base = baseName( file ).replace( ".png", "" ).replace( ".svg", "" ).replace( ".xpm", "" ).replace( ".svgz", "" );
			QStringList paths = idb.value( base, QStringList() ).toStringList();
			paths << file;

			idb.setValue( base, paths );
		}
	}

	/* Clear our older mimetype database */
	mdb.clear();

	/* Create our mimetype database */
	Q_FOREACH( QMimeType mType, mimeDb.allMimeTypes() ) {
		QString mIcon = mType.iconName();
		QString gmIcon = mType.genericIconName();

		QStringList paths;

		paths << icon( mIcon );
		if ( not paths.count() )
			paths << icon( gmIcon );

		else if ( paths.at( 0 ) == ":/icons/unknown.png" ) {
			paths.clear();
			paths << icon( gmIcon );
		}

		if ( paths.count() )
			mdb.setValue( mType.name(), paths );

		else
			mdb.setValue( mType.name(), QStringList() << ":/icons/unknown.png" );
	}

	/* Sync our database */
	mdb.sync();

	/* DJVU Fix */
	mdb.setValue( "image/vnd.djvu+multipage", QStringList() << icon( "x-office-document" ) );

	/* Executable Fix */
	mdb.setValue( "application/x-executable", QStringList() << ":/icons/exec.png" );

	/* Shared Object Fix */
	mdb.setValue( "application/x-sharedlib", QStringList() << ":/icons/binary.png" );

	/* APK Fix */
	mdb.setValue( "application/vnd.android.package-archive", icon( "package-x-generic" ) );

	/* Desktop file Fix */
	mdb.setValue( "application/x-desktop", QStringList() << ":/icons/exec.png" );

	/* DOCX Fix */
	mdb.setValue( "application/vnd.openxmlformats-officedocument.wordprocessingml.document", icon( "application-vnd.ms-word" ) );

	/* PPTX Fix */
	mdb.setValue( "application/vnd.openxmlformats-officedocument.presentationml.presentation", icon( "application-vnd.ms-powerpoint" ) );

	/* XLSX Fix */
	mdb.setValue( "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", icon( "application-vnd.ms-excel" ) );
};

/*
	*
	* NBTools.cpp - Some tools for NewBreeze
	*
*/

#include <NBTools.hpp>
#include <NBStyleManager.hpp>

QDir NBDir( QString path ) {

	QDir dir( path );
	dir.makeAbsolute();
	dir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
	dir.setSorting( QDir::DirsFirst | QDir::IgnoreCase );

	return dir;
};

QString dirName( QString path ) {

	return QString( dirname( strdup( qPrintable( path ) ) ) );
};

QString baseName( QString path ) {

	return QString( basename( strdup( qPrintable( path ) ) ) );
};

QString getMimeType( QString path ) {

	magic_t mgcMime = magic_open( MAGIC_MIME_TYPE );
	magic_load( mgcMime, NULL );

	return QString( magic_file( mgcMime, qPrintable( path ) ) );
};

inline QMimeType getMime( QString path ) {

	return mimeDb.mimeTypeForFile( path );
};

QString getMimeTypeAlt( QString path ) {

	return mimeDb.mimeTypeForFile( path ).name();
};

QStringList getDesktopEntries( QString path ) {

	QSettings dCache( mimeProgsCache, QSettings::NativeFormat );

	QMimeType mime = getMime( path );

	QStringList entriesList;
	QStringList mimeTypes = QStringList() << mime.name() << mime.allAncestors();

	foreach( QString mimeType, mimeTypes )
		entriesList << dCache.value( mimeType ).toStringList();

	entriesList.removeDuplicates();
	entriesList.sort();

	return entriesList;
};

QString termFormatString( QString file ) {

	QString chars = "!\"#$%&'()*+,:;<=>?@[\\]^`{|}~ ";

	foreach( QString ch, chars )
		file.replace( ch, "\\" + ch );

	return file;
};

bool isFile( QString path ) {

	struct stat statbuf;
	if ( stat( qPrintable( path ), &statbuf ) == 0 )

		if ( S_ISREG( statbuf.st_mode ) or S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool isDir( QString path ) {

	struct stat statbuf;
	if ( stat( qPrintable( path ), &statbuf ) == 0 )

		if ( S_ISDIR( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool isLink( QString path ) {

	struct stat statbuf;
	if ( lstat( qPrintable( path ), &statbuf ) == 0 )
		if ( S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool exists( QString path ) {
	struct stat fileAtt;

	if ( stat( qPrintable( path ), &fileAtt ) == 0 )
		return true;

	else
		return false;

}

QString readLink( QString path ) {

	char linkTarget[ 1024 ] = { 0 };
	readlink( qPrintable( path ), linkTarget, 1023 );

	return QString( linkTarget );
};

bool removeDir( QString dirName ) {

    bool result = true;
    QDir dir( dirName);

    if ( dir.exists( dirName ) ) {
        Q_FOREACH( QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst ) ) {
            if ( info.isDir() )
                result = removeDir( info.absoluteFilePath() );

            else
                result = QFile::remove( info.absoluteFilePath() );

            if ( !result )
                return result;
        }
        result = dir.rmdir( dirName );
    }

    return result;
};

bool isReadable( QString path ) {

	/*
		*
		* If the path is a folder, then both read bit and exec bit must be set
		* If the path is a file, then only read bit is sufficient
		*
	*/

	// FIXME: Check the validity of this code below: L209

	struct stat fileMode;
	QList<int> groupList;
	gid_t *groups = 0;

	if ( stat( qPrintable( path ), &fileMode ) != 0 )
		return false;

	int ngrps = getgroups( getgroups( 0, groups ), groups );
	if ( ngrps > 0 )
		for( int i = 0; i < ngrps; i++ )
			groupList << groups[ i ];

	if ( geteuid() == fileMode.st_uid ) {
		if ( S_ISDIR( fileMode.st_mode ) ) {
			if ( ( fileMode.st_mode & S_IRUSR ) and ( fileMode.st_mode & S_IXUSR ) )
				return true;

			else
				return false;
		}

		else {
			if ( fileMode.st_mode & S_IRUSR )
				return true;

			else
				return false;
		}
	}

	// Possible erroneous code
	else if ( groupList.contains( fileMode.st_gid ) ) {
		if ( S_ISDIR( fileMode.st_mode ) ) {
			if ( ( fileMode.st_mode & S_IRGRP ) and ( fileMode.st_mode & S_IXGRP ) )
				return true;

			else
				return false;
		}

		else {
			if ( fileMode.st_mode & S_IRGRP )
				return true;

			else
				return false;
		}
	}

	else {
		if ( S_ISDIR( fileMode.st_mode ) ) {
			if ( ( fileMode.st_mode & S_IROTH ) and ( fileMode.st_mode & S_IXOTH ) )
				return true;

			else
				return false;
		}

		else {
			if ( fileMode.st_mode & S_IROTH )
				return true;

			else
				return false;
		}
	}

	return false;
};

qint64 nChildren( QString path ) {

	qint64 entries = 0;
	struct dirent *ent;
	DIR *dir = opendir( qPrintable( path ) );

	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL)
			entries++;

		// Remove entries corresponding to . and ..
		entries -= 2;
		closedir( dir );
	}

	return entries;
};

qint64 getSize( QString path ) {

	struct stat statbuf;
	if ( stat( qPrintable( path ), &statbuf ) == 0 )
		return statbuf.st_size;

	else
		return 0;
};

mode_t getMode( QString path ) {

	struct stat fileAtts;
	if ( stat( qPrintable( path ), &fileAtts ) != 0 ) {
		return -1;
	}

	return fileAtts.st_mode;
};

QStringList recDirWalk( QString path ) {

	QStringList fileList;

	if ( not QFileInfo( path ).exists() )
		return QStringList();

	QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::Hidden, QDirIterator::Subdirectories );
	while ( it.hasNext() ) {
		it.next();
		if( it.fileInfo().isDir() ) {
			fileList.append( it.fileInfo().filePath() );
		}

		else {
			fileList.append( it.fileInfo().filePath() );
		}
	}

	return fileList;
};

bool isText( QString path ) {

	QStringList plainMimes;
	plainMimes << "application/javascript"
				<< "application/x-javascript"
				<< "application/xml"
				<< "application/x-csh'"
				<< "application/x-sh"
				<< "application/x-shellscript"
				<< "application/javascript"
				<< "application/x-perl"
				<< "application/x-php"
				<< "application/x-ruby";

	if ( ( plainMimes.count( getMimeTypeAlt( path ) ) ) > 0 )
		return true;

	else if ( getMimeTypeAlt( path ).startsWith( "text" ) )
		return true;

	return false;
};

bool isExec( QString path ) {

	QStringList execMimes;
	execMimes << "application/x-sharedlib"
				<< "application/x-executable"
				<< "application/x-shellscript"
				<< "application/x-install";

	if ( execMimes.contains( getMimeTypeAlt( path ) ) )
		return true;

	if ( QFileInfo( path ).isExecutable() )
		return true;

	return false;
};

bool isImage( QString path ) {

	QString mime = getMimeTypeAlt( path );

	// image/vnd.djvu, image/x-djvu: May not be an image but a Multipage DjVu Document
	// So we ignore this. Anyway, no thumbnailing djvu documents is done at all.
	if ( mime.contains( "djvu" )  )
		return false;

	if ( mime.startsWith( "image" ) or mime.contains( "gif" ) or mime.contains( "mng" ) )
		return true;

	return false;
};

bool isArchive( QString path ) {

	QStringList archiveMimes;
	archiveMimes << "application/x-7z-compressed" << "application/x-ace" << "application/x-alz"
				<< "application/x-ar" << "application/x-arj" << "application/x-stuffit"
				<< "application/x-bzip" << "application/x-bzip2" << "application/vnd.ms-cab-compressed"
				<< "application/x-cbr" << "application/x-cbz" << "application/x-cpio"
				<< "application/x-deb" << "application/x-ear" << "application/x-ms-dos-executable"
				<< "application/x-gzip" << "application/x-cd-image" << "application/x-java-archive"
				<< "application/x-lha" << "application/x-lha" << "application/x-lzip"
				<< "application/x-lzma" << "application/x-lzop" << "application/x-rar"
				<< "application/x-rpm" << "application/x-rzip" << "application/x-stuffit"
				<< "application/x-tar" << "application/x-bzip-compressed-tar"
				<< "application/x-compressed-tar" << "application/x-lzip-compressed-tar"
				<< "application/x-lzma-compressed-tar" << "application/x-lzop-compressed-tar"
				<< "application/x-7z-compressed-tar" << "application/x-xz-compressed-tar"
				<< "application/x-tarz" << "application/x-tarz" << "application/x-compressed-tar"
				<< "application/x-xz-compressed-tar" << "application/x-lzip-compressed-tar"
				<< "application/x-lzma-compressed-tar" << "application/x-lzop-compressed-tar"
				<< "application/x-war" << "application/x-xz" << "application/x-gzip"
				<< "application/x-compress" << "application/zip" << "application/x-zoo";

	return archiveMimes.contains( getMimeTypeAlt( path ) );
}

bool isArchiveAlt( QString mimeType ) {

	QStringList archiveMimes;
	archiveMimes << "application/x-7z-compressed" << "application/x-ace" << "application/x-alz"
				<< "application/x-ar" << "application/x-arj" << "application/x-stuffit"
				<< "application/x-bzip" << "application/x-bzip2" << "application/vnd.ms-cab-compressed"
				<< "application/x-cbr" << "application/x-cbz" << "application/x-cpio"
				<< "application/x-deb" << "application/x-ear" << "application/x-ms-dos-executable"
				<< "application/x-gzip" << "application/x-cd-image" << "application/x-java-archive"
				<< "application/x-lha" << "application/x-lha" << "application/x-lzip"
				<< "application/x-lzma" << "application/x-lzop" << "application/x-rar"
				<< "application/x-rpm" << "application/x-rzip" << "application/x-stuffit"
				<< "application/x-tar" << "application/x-compressed-tar"
				 << "application/x-bzip-compressed-tar" << "application/x-lzip-compressed-tar"
				<< "application/x-lzma-compressed-tar" << "application/x-lzop-compressed-tar"
				<< "application/x-7z-compressed-tar" << "application/x-xz-compressed-tar"
				<< "application/x-tarz" << "application/x-tarz" << "application/x-compressed-tar"
				<< "application/x-xz-compressed-tar" << "application/x-lzip-compressed-tar"
				<< "application/x-lzma-compressed-tar" << "application/x-lzop-compressed-tar"
				<< "application/x-war" << "application/x-xz" << "application/x-gzip"
				<< "application/x-compress" << "application/zip" << "application/x-zoo";

	return archiveMimes.contains( mimeType );
};

QString formatSize( qint64 num ) {

	QString total;
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;

	if ( num >= tb ) total = QString( "%1 TiB" ).arg( QString::number( qreal( num ) / tb, 'f', 3 ) );
	else if ( num >= gb ) total = QString( "%1 GiB" ).arg( QString::number( qreal( num ) / gb, 'f', 2 ) );
	else if ( num >= mb ) total = QString( "%1 MiB" ).arg( QString::number( qreal( num ) / mb, 'f', 1 ) );
	else if ( num >= kb ) total = QString( "%1 KiB" ).arg( QString::number( qreal( num ) / kb,'f',1 ) );
	else total = QString( "%1 bytes" ).arg( num );

	return total;
};

QString getStyleSheet( QString Widget, QString Style ) {

	return styleManager.getStyleSheet( Widget, Style );
};

QStringList getTerminal() {

	QSettings sett;
	if ( !sett.value( "terminals/nbterminal" ).toStringList().isEmpty() )
		return sett.value( "terminals/nbterminal" ).toStringList();

	if ( !sett.value( "terminals/konsole" ).toStringList().isEmpty() )
		return sett.value( "terminals/konsole" ).toStringList();

	if ( !sett.value( "terminals/gnome-terminal" ).toStringList().isEmpty() )
		return sett.value( "terminals/gnome-terminal" ).toStringList();

	if ( !sett.value( "terminals/mate-terminal" ).toStringList().isEmpty() )
		return sett.value( "terminals/mate-terminal" ).toStringList();

	if ( !sett.value( "terminals/xfce4-terminal" ).toStringList().isEmpty() )
		return sett.value( "terminals/xfce4-terminal" ).toStringList();

	if ( !sett.value( "terminals/xterm" ).toStringList().isEmpty() )
		return sett.value( "terminals/xterm" ).toStringList();

	return QStringList();
};

void NBMessageOutput( QtMsgType type, const char* message ) {

	switch ( type ) {

		case QtDebugMsg: {
			fprintf( stderr, "\033[01;30mNewBreeze::Debug# %s\n\033[00;00m", message );
			break;
		}

		case QtWarningMsg: {
			fprintf( stderr, "\033[01;33mNewBreeze::Warning# %s\n\033[00;00m", message );
			break;
		}

		case QtCriticalMsg: {
			fprintf( stderr, "\033[01;31mNewBreeze::CriticalError# %s\n\033[00;00m", message );
			break;
		}

		case QtFatalMsg: {
			fprintf( stderr, "\033[01;41mNewBreeze::FatalError# %s\n\033[00;00m", message );
			abort();
		}
	}
};

void NBDebugMsg(  DbgMsgPart::MsgPart part, const char *format, ... ) {

	switch ( part ) {
		case DbgMsgPart::HEAD: {
			fprintf( stderr, "\033[01;30mNewBreeze::Debug# " );
			va_list args;
			va_start( args, format );
			vfprintf( stderr, format, args );
			va_end( args );

			break;
		}

		case DbgMsgPart::BODY: {
			va_list args;
			va_start( args, format );
			vfprintf( stderr, format, args );
			va_end( args );

			break;
		}

		case DbgMsgPart::TAIL: {
			va_list args;
			va_start( args, format );
			vfprintf( stderr, format, args );
			va_end( args );
			fprintf( stderr, "\n\033[00;00m" );

			break;
		}

		case DbgMsgPart::ONESHOT: {
			fprintf( stderr, "\033[01;30mNewBreeze::Debug# " );
			va_list args;
			va_start( args, format );
			vfprintf( stderr, format, args );
			va_end( args );
			fprintf( stderr, "\n\033[00;00m" );

			break;
		}
	}

	fflush( stderr );
};

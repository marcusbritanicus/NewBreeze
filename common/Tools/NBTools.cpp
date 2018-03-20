/*
	*
	* NBTools.cpp - Some tools for NewBreeze
	*
*/

#include "NBTools.hpp"

static QMimeDatabase mimeDb;

QString dirName( QString path ) {

	if ( path == "/" or path == "//" )
		return "/";

	/* Simple clean path: remove '//' and './' */
	path = path.replace( "//", "/" ).replace( "/./", "/" );

	char *dupPath = strdup( path.toLocal8Bit().constData() );
	QString dirPth = QString::fromLocal8Bit( dirname( dupPath ) );
	dirPth += ( dirPth.endsWith( "/" ) ? "" : "/" );
	free( dupPath );

	return dirPth;
};

QString baseName( QString path ) {

	if ( path == "/" or path == "//" )
		return "/";

	/* Simple clean path" remove '//' and './' */
	path = path.replace( "//", "/" ).replace( "/./", "/" );

	char *dupPath = strdup( path.toLocal8Bit().constData() );
	QString basePth = QString::fromLocal8Bit( basename( dupPath ) );
	free( dupPath );

	return basePth;
};

QString getMimeType( QString path ) {

	return mimeDb.mimeTypeForFile( path ).name();
};

QString termFormatString( QString file ) {

	QString chars = "!\"#$%&'()*+,:;<=>?@[\\]^`{|}~ ";

	foreach( QString ch, chars )
		file.replace( ch, "\\" + ch );

	return file;
};

QString MD5( QString data ) {

	return QString::fromLocal8Bit( QCryptographicHash5::hash( data.toLocal8Bit(), QCryptographicHash5::Md5 ).toHex() );
};

bool isFile( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().constData(), &statbuf ) == 0 )

		if ( S_ISREG( statbuf.st_mode ) or S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool isDir( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().constData(), &statbuf ) == 0 )

		if ( S_ISDIR( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool isLink( QString path ) {

	struct stat statbuf;
	if ( lstat( path.toLocal8Bit().constData(), &statbuf ) == 0 )
		if ( S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool exists( QString path ) {

	return not access( path.toLocal8Bit().constData(), F_OK );
}

QString readLink( QString path ) {

	char linkTarget[ PATH_MAX ] = { 0 };
	realpath( path.toLocal8Bit().constData(), linkTarget );

	return QString( linkTarget );
};

int mkpath( QString path, mode_t mode ) {

	/* Root always exists */
	if ( path == "/" )
		return 0;

	/* If the directory exists, thats okay for us */
	if ( exists( path ) )
		return 0;

	mkpath( dirName( path ), mode );

	return mkdir( path.toLocal8Bit().constData(), mode );
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

	if ( isDir( path ) )
		return not access( path.toLocal8Bit().constData(), R_OK | X_OK );

	else
		return not access( path.toLocal8Bit().constData(), R_OK );
};

bool isWritable( QString path ) {

	return not access( path.toLocal8Bit().constData(), W_OK );
};

bool isExecutable( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) != 0 )
		return false;

	if ( ( statbuf.st_mode & S_IXUSR ) ) {
		QMimeType m = mimeDb.mimeTypeForFile( path );
		if ( m.name() == "application/x-executable" )
			return true;

		else if ( m.name() == "application/x-sharedlib" )
			return true;

		else if ( m.allAncestors().contains( "application/x-executable" ) )
			return true;

		/* Default is false */
		return false;
	}

	return false;
};

qint64 nChildren( QString path ) {

	qint64 entries = 0;
	struct dirent *ent;
	DIR *dir = opendir( path.toLocal8Bit().constData() );

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
	if ( stat( path.toLocal8Bit().constData(), &statbuf ) != 0 )
		return 0;

	switch( statbuf.st_mode & S_IFMT ) {
		case S_IFREG: {

			return statbuf.st_size;
		}

		case S_IFDIR: {
			DIR* d_fh;
			struct dirent* entry;
			QString longest_name;

			while ( ( d_fh = opendir( path.toLocal8Bit().constData() ) ) == NULL ) {
				qWarning() << "Couldn't open directory:" << path;
				return statbuf.st_size;
			}

			quint64 size = statbuf.st_size;

			longest_name = QString( path );
			if ( not longest_name.endsWith( "/" ) )
				longest_name += "/";

			while( ( entry = readdir( d_fh ) ) != NULL ) {

				/* Don't descend up the tree or include the current directory */
				if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

					if ( entry->d_type == DT_DIR ) {

						/* Recurse into that folder */
						size += getSize( longest_name + entry->d_name );
					}

					else {

						/* Get the size of the current file */
						size += getSize( longest_name + entry->d_name );
					}
				}
			}

			closedir( d_fh );
			return size;
		}

		default: {

			/* Return 0 for all other nodes: chr, blk, lnk, symlink etc */
			return 0;
		}
	}

	/* Should never come till here */
	return 0;
};

mode_t getMode( QString path ) {

	struct stat fileAtts;
	if ( stat( path.toLocal8Bit().constData(), &fileAtts ) != 0 ) {
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

	QMimeType mime = mimeDb.mimeTypeForFile( path );
	if ( mime.name() == "text/plain" )
		return true;

	if ( mime.allAncestors().contains( "text/plain" ) )
		return true;

	return false;
};

bool isExec( QString path ) {

	/* If the exec bit is not set there is no point continuing */
	if ( access( path.toLocal8Bit().constData(), X_OK ) )
		return false;

	QMimeType mType = mimeDb.mimeTypeForFile( path );
	/* If this is an application/x-executable, it can be executed */
	if ( ( mType.name() == "application/x-executable" ) )
		return true;

	/* There might be a file, say a .'odt' or 'txt' with exec bit set. Rule it out */
	/* So if this has application/x-executable in its ancestry tree, then it can be executed */
	if ( mType.allAncestors().contains( "application/x-executable" ) )
		return true;

	/* Some shared libraries, and a few other file can also be executed */
	QStringList execMimes;
	execMimes << "application/x-sharedlib"
				<< "application/x-install";

	/* We'll execute them */
	if ( execMimes.contains( mType.name() ) )
		return true;

	return false;
};

bool isImage( QString path ) {

	QString mime = getMimeType( path );

	// image/vnd.djvu, image/x-djvu: May not be an image but a Multipage DjVu Document
	// So we ignore this. Anyway, no thumbnailing djvu documents is done at all.
	if ( mime.contains( "djvu" )  )
		return false;

	if ( mime.startsWith( "image" ) or mime.contains( "gif" ) or mime.contains( "mng" ) )
		return true;

	return false;
};

bool isArchive( QString path ) {

	return isArchiveAlt( getMimeType( path ) );
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
	else total = QString( "%1 byte%2" ).arg( num ).arg( num > 1 ? "s": "" );

	return total;
};

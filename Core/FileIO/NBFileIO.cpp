/*
	*
	* NBFileIOManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>

#define COPY_BUF_SIZE ( 64 * 1024 )

/*
	*
	* NBFileIO Initialization.
	*
	* @v mode is set to NBIOMode::Copy
	* @v errorNodes and @v sourceList lists are cleared
	* @v targetDir and @v ioTarget are set to empty strings
	* All sizes are set to 0
	* @v wasCanceled and @v isPaused set to false
	*
*/
NBFileIO::NBFileIO() {

	jobID = MD5( QDateTime::currentDateTime().toString( Qt::ISODate ) );

	errorNodes.clear();
	sourceList.clear();
	targetDir = QString();

	ioTarget = QString();

	mode = NBIOMode::Copy;

	totalSize = 0;
	copiedSize = 0;
	fTotalBytes = 0;
	fWritten = 0;
};

void NBFileIO::setSources( QStringList sources ) {

	sourceList << sources;
};

QStringList NBFileIO::sources() {

	return sourceList;
};

void NBFileIO::setTarget( QString target ) {

	targetDir = target.endsWith( "/" ) ? target : target + "/";
	jobID = MD5( targetDir + QDateTime::currentDateTime().toString( Qt::ISODate ) );
};

QString NBFileIO::target() {

	return targetDir;
};

void NBFileIO::setIOMode( NBIOMode::Mode io_Mode ) {

	mode = io_Mode;
};

NBIOMode::Mode NBFileIO::ioMode() {

	return mode;
};

void NBFileIO::cancel() {

	wasCanceled = true;
};

void NBFileIO::pause() {

	isPaused = true;
};

void NBFileIO::resume() {

	isPaused = false;
};

const QString NBFileIO::id() {

	return jobID;
};

int NBFileIO::result() {

	if ( errorNodes.count() )
		return 1;

	else
		return 0;
};

QStringList NBFileIO::errors() {

	return errorNodes;
};

void NBFileIO::performIO() {

	preIO();

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		if ( wasCanceled )
			break;

		while ( isPaused ) {
			if ( wasCanceled )
				break;

			usleep( 100 );
			qApp->processEvents();
		}

		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) )
			copyDir( srcBase );

		else
			copyFile( srcBase );

		chdir( qPrintable( curWD ) );
	}

	emit IOComplete();
};

void NBFileIO::preIO() {

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, sourceList ) {
		QString srcPath = dirName( node );
		QString srcBase = baseName( node );

		chdir( qPrintable( srcPath ) );
		if ( isDir( node ) ) {
			/*
				*
				* This is a top level directory. This won't be created in @f getDirSize( ... )
				* We need to create this directory and get its size.
				*
			*/

			/* If we are moving and its an intra-device move then mkpath won't be necessary */
			struct stat iStat, oStat;
			stat( qPrintable( node ), &iStat );
			stat( qPrintable( targetDir ), &oStat );

			if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) )
				continue;

			else {
				mkpath( srcBase, QFile::permissions( srcBase ) );
				getDirSize( srcBase );
			}
		}

		else {
			// This is a file. Just get its size
			totalSize += getSize( srcBase );
		}

		chdir( qPrintable( curWD ) );
	}
};

void NBFileIO::getDirSize( QString path ) {

	DIR* d_fh;
	struct dirent* entry;
	QString longest_name;

	while( ( d_fh = opendir( qPrintable( path ) ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	longest_name = QString( path );
	if ( not longest_name.endsWith( "/" ) )
		longest_name += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/*
					*
					* Prepend the current directory and recurse
					* We also need to create this directory in
					* @v targetDir
					*
				*/
				mkpath( longest_name + entry->d_name, QFile::permissions( longest_name + entry->d_name ) );
				getDirSize( longest_name + entry->d_name );
			}
			else {

				/* Copy the current file */
				totalSize += getSize( longest_name + entry->d_name );
			}
		}
	}

	closedir( d_fh );
};

void NBFileIO::copyFile( QString srcFile ) {

	ioTarget = targetDir + srcFile;

	if ( not isReadable( srcFile ) ) {
		qDebug() << "Unreadable file: " + srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( ioTarget ) ) ) {
		qDebug() << ioTarget + " not writable!!!";
		errorNodes << srcFile;
		// return;
	}

	if ( exists( ioTarget ) ) {
		QString suffix = mimeDb.suffixForFileName( srcFile );
		ioTarget = ( suffix.length() ? ioTarget.left( ioTarget.length() - ( 1 + suffix.length() ) ) + " - Copy." + suffix : ioTarget + " - Copy" );
	}

	struct stat iStat, oStat;
	stat( qPrintable( srcFile ), &iStat );
	stat( qPrintable( targetDir ), &oStat );

	/* If the operation is intra-device operation and its a move, then we can simply rename the file */
	if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) ) {
		QFile::rename( srcFile, ioTarget );
		return;
	}

	int iFileFD = open( qPrintable( srcFile ), O_RDONLY );
	int oFileFD = open( qPrintable( ioTarget ), O_WRONLY | O_CREAT );

	fTotalBytes = iStat.st_size;
	fWritten = 0;
	int bytesWritten = 0;

	while( fWritten != quint64( iStat.st_size ) ) {
		if ( wasCanceled ) {
			close( iFileFD );
			close( oFileFD );

			return;
		}

		while ( isPaused ) {
			if ( wasCanceled ){
				close( iFileFD );
				close( oFileFD );

				return;
			}

			usleep( 100 );
			qApp->processEvents();
		}

		char block[ COPY_BUF_SIZE ];
		qint64 inBytes = read( iFileFD, block, sizeof( block ) );
		bytesWritten = write( oFileFD, block, inBytes );
		fWritten += bytesWritten;
		copiedSize += bytesWritten;
		qApp->processEvents();
	}

	close( iFileFD );
	close( oFileFD );

	QFile::setPermissions( ioTarget, QFile::permissions( srcFile ) );

	if ( mode == NBIOMode::Move ) {
		if ( fWritten == quint64( iStat.st_size ) )
			unlink( qPrintable( srcFile) );

		else
			errorNodes << srcFile;
	}
};

void NBFileIO::copyDir( QString path ) {

	/* If the operation is intra-device operation and its a move, then we can simply rename the file */
	struct stat iStat, oStat;
	stat( qPrintable( path ), &iStat );
	stat( qPrintable( targetDir ), &oStat );

	if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) ) {
		/*
			*
			* If the rename fails, then the target may be existing and is not empty
			* In such a case, we must perform a normal copy
			*
		*/
		if ( not rename( qPrintable( path ), qPrintable( targetDir + baseName( path ) ) ) )
			return;
	}

	DIR* d_fh;
	struct dirent* entry;
	QString longest_name;

	if ( wasCanceled )
		return;

	while( ( d_fh = opendir( qPrintable( path ) ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	longest_name = QString( path );
	if ( not longest_name.endsWith( "/" ) )
		longest_name += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			/* If it's a directory print it's name and recurse into it */
			if ( entry->d_type == DT_DIR ) {

				/* Prepend the current directory and recurse */
				copyDir( longest_name + entry->d_name );
			}
			else {

				/* Copy the current file */
				copyFile( longest_name + entry->d_name );
			}
		}
	}

	closedir( d_fh );

	if ( mode == NBIOMode::Move )
		rmdir( qPrintable( path ) );
};

void NBFileIO::mkpath( QString path, QFile::Permissions dirPerms ) {

	QString curWD( get_current_dir_name() );

	chdir( qPrintable( targetDir ) );
	QString __path;
	Q_FOREACH( QString pathBit, path.split( "/", QString::SkipEmptyParts ) ) {
		__path += pathBit + "/";
		if ( not exists( __path ) ) {
			mkdir( qPrintable( __path ), S_IRWXU );
			QFile::setPermissions( __path, dirPerms );
		}
	}
	chdir( qPrintable( curWD ) );
};

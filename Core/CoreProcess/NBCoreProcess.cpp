/*
	*
	* NBCoreProcess.cpp - NewBreeze Core Process Class for I/O, permission settings, etc
	*
*/

#include <NBCoreProcess.hpp>

#define COPY_BUF_SIZE ( 64 * 1024 )

NBIOProcess::NBIOProcess() {

	jobID = MD5( QDateTime::currentDateTime().toString( Qt::ISODate ) );

	wasCanceled = false;
	isPaused = false;

	errorNodes.clear();

	mProgress.currentFile = QString();

	mode = NBIOMode::Copy;

	mProgress.sourceList = QStringList();
	mProgress.targetDir = QString();
	mProgress.totalBytes = 0;
	mProgress.totalBytesCopied = 0;
	mProgress.currentFile = QString();
	mProgress.fileBytes = 0;
	mProgress.fileBytesCopied = 0;
};

void NBIOProcess::setSources( QStringList sources ) {

	mProgress.sourceList << sources;
};

QStringList NBIOProcess::sources() {

	return mProgress.sourceList;
};

void NBIOProcess::setTarget( QString target ) {

	mProgress.targetDir = target.endsWith( "/" ) ? target : target + "/";
	jobID = MD5( mProgress.targetDir + QDateTime::currentDateTime().toString( Qt::ISODate ) );
};

QString NBIOProcess::target() {

	return mProgress.targetDir;
};

void NBIOProcess::setIOMode( NBIOMode::Mode io_Mode ) {

	mode = io_Mode;
};

NBIOMode::Mode NBIOProcess::ioMode() {

	return mode;
};

void NBIOProcess::cancel() {

	wasCanceled = true;
};

void NBIOProcess::pause() {

	isPaused = true;
};

void NBIOProcess::resume() {

	isPaused = false;
};

Progress progress() {

	return mProgress
}

QStringList NBIOProcess::errors() {

	return errorNodes;
};

void NBIOProcess::run() {

	preIO();

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, mProgress.sourceList ) {
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
};

void NBIOProcess::preIO() {

	QString curWD( get_current_dir_name() );

	Q_FOREACH( QString node, mProgress.sourceList ) {
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
			stat( qPrintable( mProgress.targetDir ), &oStat );

			if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) )
				continue;

			else {
				mkpath( srcBase, QFile::permissions( srcBase ) );
				getDirSize( srcBase );
			}
		}

		else {
			// This is a file. Just get its size
			mProgress.totalBytes += getSize( srcBase );
		}

		chdir( qPrintable( curWD ) );
	}
};

void NBIOProcess::getDirSize( QString path ) {

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
					* @v mProgress.targetDir
					*
				*/
				mkpath( longest_name + entry->d_name, QFile::permissions( longest_name + entry->d_name ) );
				getDirSize( longest_name + entry->d_name );
			}
			else {

				/* Copy the current file */
				mProgress.totalBytes += getSize( longest_name + entry->d_name );
			}
		}
	}

	closedir( d_fh );
};

void NBIOProcess::copyFile( QString srcFile ) {

	mProgress.currentFile = mProgress.targetDir + srcFile;

	if ( not isReadable( srcFile ) ) {
		qDebug() << "Unreadable file: " + srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( mProgress.currentFile ) ) ) {
		qDebug() << mProgress.currentFile + " not writable!!!";
		errorNodes << srcFile;
		// return;
	}

	if ( exists( mProgress.currentFile ) ) {
		QString suffix = mimeDb.suffixForFileName( srcFile );
		mProgress.currentFile = ( suffix.length() ? mProgress.currentFile.left( mProgress.currentFile.length() - ( 1 + suffix.length() ) ) + " - Copy." + suffix : mProgress.currentFile + " - Copy" );
	}

	struct stat iStat, oStat;
	stat( qPrintable( srcFile ), &iStat );
	stat( qPrintable( mProgress.targetDir ), &oStat );

	/* If the operation is intra-device operation and its a move, then we can simply rename the file */
	if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) ) {
		QFile::rename( srcFile, mProgress.currentFile );
		return;
	}

	int iFileFD = open( qPrintable( srcFile ), O_RDONLY );
	int oFileFD = open( qPrintable( mProgress.currentFile ), O_WRONLY | O_CREAT, 0644 );

	mProgress.fileBytes = iStat.st_size;
	mProgress.fileBytesCopied = 0;
	int bytesWritten = 0;

	while( mProgress.fileBytesCopied != quint64( iStat.st_size ) ) {
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
		mProgress.fileBytesCopied += bytesWritten;
		mProgress.totalBytesCopied += bytesWritten;
		qApp->processEvents();
	}

	close( iFileFD );
	close( oFileFD );

	QFile::setPermissions( mProgress.currentFile, QFile::permissions( srcFile ) );

	if ( mode == NBIOMode::Move ) {
		if ( mProgress.fileBytesCopied == quint64( iStat.st_size ) )
			unlink( qPrintable( srcFile) );

		else
			errorNodes << srcFile;
	}
};

void NBIOProcess::copyDir( QString path ) {

	/* If the operation is intra-device operation and its a move, then we can simply rename the file */
	struct stat iStat, oStat;
	stat( qPrintable( path ), &iStat );
	stat( qPrintable( mProgress.targetDir ), &oStat );

	if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) ) {
		/*
			*
			* If the rename fails, then the target may be existing and is not empty
			* In such a case, we must perform a normal copy
			*
		*/
		if ( not rename( qPrintable( path ), qPrintable( mProgress.targetDir + baseName( path ) ) ) )
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

void NBIOProcess::mkpath( QString path, QFile::Permissions dirPerms ) {

	QString curWD( get_current_dir_name() );

	chdir( qPrintable( mProgress.targetDir ) );
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

/*
	*
	* NBIOProcess.cpp - NewBreeze Process Class for file I/O
	*
	* This is the heart of copy/move operations.
	*
	* NBProcess::Progress is defined in the header NBAbstractProcess.hpp
	* It contains all the necessary variables to show the progress dialog.
	*
	* Firstly we check if the target provided to us is writable. If it is not writable we
	* emit noWriteAccess() and end the process.
	*
	* Then, we check if are moving inside a device. If we are, no processing will take place.
	* Otherwise, we process the sources. The processing just involves listing of all the nodes
	* living under the given sources, if any of them are directories. We also compute the size
	* of the file to be copied, and create the source directory tree at the target as we list
	* the directories.
	*
	* All the types of nodes will be listed for the IO process. Some like socket may not be
	* recreated during the IO.
	*
	* Once this is done, we check if the target has enough space. If not, we emit noSpace()
	* and end the process.
	*
*/

#include <NBIOProcess.hpp>

NBIOProcess::NBIOProcess( QStringList sources, NBProcess::Progress *progress ) {

	origSources.clear();
	origSources << sources;

	sourceList.clear();

	mProgress = progress;

	if ( not mProgress->sourceDir.endsWith( "/" ) )
		mProgress->sourceDir+= "/";

	if ( not mProgress->targetDir.endsWith( "/" ) )
		mProgress->targetDir+= "/";

	/* Initialize the sizes to zero */
	mProgress->totalBytes = 0;
	mProgress->totalBytesCopied = 0;
	mProgress->fileBytes = 0;
	mProgress->fileBytesCopied = 0;

	/* Initialize the state to NBProcess::NotStarted */
	mProgress->state = NBProcess::NotStarted;
};

QStringList NBIOProcess::errors() {

	return errorNodes;
};

void NBIOProcess::cancel() {

	mProgress->state = NBProcess::Canceled;
	mCanceled = true;
};

void NBIOProcess::pause() {

	mProgress->state = NBProcess::Paused;
	mPaused = true;
};

void NBIOProcess::resume() {

	mProgress->state = NBProcess::Started;
	mPaused = false;
};

bool NBIOProcess::canUndo() {

	return ( exists( mProgress->sourceDir ) and exists( mProgress->targetDir ) );
};

void NBIOProcess::undo() {

	/* If we copied the nodes, then delete them */
	if ( mProgress->type == NBProcess::Copy ) {
		mUndo = true;
		start();
	};

	/* We moved the files, move them back */
	else {
		mProgress->sourceDir.swap( mProgress->targetDir );
		start();
	}
};

void NBIOProcess::preIO() {

	mProgress->state = NBProcess::Starting;

	if ( not isWritable( mProgress->targetDir ) ) {
		emit noWriteAccess();
		return false;
	}

	/* If we are moving file and in the same partition, we can skip the processing */
	if ( mProgress->type == NBProcess::Move ) {

		struct stat srcStat, tgtStat;
		stat( mProgress->sourceDir.toLocal8Bit().data(), &srcStat );
		stat( mProgress->targetDir.toLocal8Bit().data(), &tgtStat );

		/* If the source and the are the same */
		if ( srcStat.st_dev == tgtStat.st_dev )
			/* Move is just a trivial rename operation, we can get out of here */
			return true;

		/* Otherwise we proceed with the source processing */
	}

	/* Store the current working directory */
	QString curWD = QString::fromLocal8Bit( get_current_dir_name() );

	/* Switch to the source directory */
	chdir( mProgress->sourceDir.toLocal8Bit().data() );

	sourceList.clear();

	/* Obtain the file sizes */
	Q_FOREACH( QString src, sourceList ) {
		mProgress->progressText = QString( "Processing %1..." ).arg( src );
		if ( isDir( src ) )
			processDirectory( src );

		else
			sourceList << src;
	}

	/* Change back to the current working directory */
	chdir( curWD.toLocal8Bit().data() );

	/* Check if we have enough space to perform the IO */
	mProgress->progressText = QString( "Checking space requirements..." );
	NBDeviceInfo tgtInfo = NBDeviceManager::deviceInfoForPath( mProgress->targetDir );
	if ( tgtInfo.availSpace() <= mProgress->totalBytes ) {
		emit noSpace();
		return false;
	}

	return true;
};

void NBIOProcess::processDirectory( QString path ) {

	DIR* d_fh;
	struct dirent* entry;

	while ( ( d_fh = opendir( path.toLocal8Bit().data() ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	quint64 size = statbuf.st_size;

	if ( not path.endsWith( "/" ) )
		path += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/* Create this directory at the target */
				mkpath( mProgress->targetDir + path + entry->d_name );

				/* Recurse into that folder */
				processDirectory( path + entry->d_name );
			}

			else {

				/* Get the size of the current file */
				mProgress->totalBytes += getSize( path + entry->d_name );

				/* Add this to the source file list */
				sourceList << path + entry->d_name;
			}
		}
	}

	closedir( d_fh );
};

void NBIOProcess::copyFile( QString srcFile ) {

	char buffer[ BUFSIZ ];

	qint64 inBytes = 0;
	qint64 bytesWritten = 0;

	mProgress->currentFile = targetDir + srcFile;

	if ( not isReadable( srcFile ) ) {
		qDebug() << "Unreadable file:" << srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( mProgress->currentFile ) ) ) {
		qDebug() << mProgress->currentFile << "not writable!!!";
		errorNodes << srcFile;
		return;
	}

	/* If the file exists, add 'Copy - ' to the beginning of the file name */
	if ( exists( mProgress->currentFile ) )
		mProgress->currentFile = dirName( mProgress->currentFile ) + "/Copy - " + baseName( mProgress->currentFile );

	struct stat iStat, oStat;
	stat( srcFile.toLocal8Bit().data(), &iStat );
	stat( targetDir.toLocal8Bit().data(), &oStat );

	/* If the operation is intra-partition operation and its a move, then we can simply rename the file */
	if ( ( iStat.st_dev == oStat.st_dev ) and ( mProgress->type == NBProcess::Move ) ) {
		if ( rename( srcFile, mProgress->currentFile ) ) {
			qDebug() << "Error moving file:" << srcFile;
			errorNodes << srcFile;
		}
		return;
	}

	/* Open the input file descriptor fro reading */
	int iFileFD = open( srcFile.toLocal8Bit().data(), O_RDONLY );

	/* Open the output file descriptor for reading */
	int oFileFD = open( targetDir.toLocal8Bit().data(), O_WRONLY | O_CREAT, iStat.st_mode );

	/* NBProcess::Progress::fileBytes */
	mProgress->fileBytes = iStat.st_size;

	/* NBProcess::Progress::fileBytesCopied */
	mProgress->fileBytesCopied = 0;

	/* While we read positive chunks of data we write it */
	while ( ( inBytes = read( iFileFD, buffer, BUFSIZ ) ) > 0 ) {
		if ( mCanceled ) {
			close( iFileFD );
			close( oFileFD );

			emit canceled( errorNodes );

			return;
		}

		while ( mPaused ) {
			if ( mCanceled ) {
				close( iFileFD );
				close( oFileFD );

				emit canceled( errorNodes );

				return;
			}

			usleep( 100 );
			qApp->processEvents();
		}

		bytesWritten = write( oFileFD, block, inBytes );

		if ( bytesWritten != inBytes ) {
			qDebug() << "Error writing to file:" << mProgress->currentFile;
			qDebug() << "[Error]:" << strerror( errno );
			errorNodes << srcFile;
			break;
		}

		mProgress->fileBytesCopied += bytesWritten;
		mProgress->totalBytesCopied += bytesWritten;

		qApp->processEvents();
	}

	close( iFileFD );
	close( oFileFD );

	/* If read(...) resulted in an error */
	if ( inBytes == -1 ) {
		qDebug() << "Error copying file:" << srcFile;
		qDebug() << "[Error]:" << strerror( errno );
		errorNodes << srcFile;
	}

	if ( mProgress->fileBytesCopied != quint64( iStat.st_size ) )
		errorNodes << srcFile;

	if ( mProgress->type == NBProcess::Move )
		/* If there were no errors encountered, and its a move operation, delete the file */
		if ( not errorNodes.contains( srcFile ) )
			unlink( srcFile.toLocal8Bit().data() );
};

void NBIOProcess::run() {

	/* Undo for NBProcess::Copy */
	if ( mUndo ) {

		mProgress->state = NBProcess::Started;

		if ( mCanceled ) {
			emit canceled( errorNodes );

			quit();
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

		errorNodes.clear();

		/* We want the progressbars to be swaying */
		mProgress->totalBytesCopied = -1;
		mProgress->fileBytesCopied = -1;

		mProgress->progressText( "Deleting files..." );
		Q_FOREACH( QString path, sourceList )
			unlink( mProgress->targetDir + path );

		mProgress->progressText( "Deleting directory tree..." );
		Q_FOREACH( QString path, origSources )
			removeDir( mProgress->targetDir + path );

		mUndo = false;

		mProgress->state = NBProcess::Complete;
		emit completed( errorNodes );

		quit();
		return;
	}

	/* Actual IO */
	if ( mCanceled ) {
		emit canceled( errorNodes );

		quit();
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

	/* First we process the sources */
	if ( not preIO() ) {

		quit();
		return;
	}

	mProgress->progressText = QString();
	mProgress->state = NBProcess::Started;

	/* Store the current working directory */
	QString curWD = QString::fromLocal8Bit( get_current_dir_name() );

	/* Switch to the source directory */
	chdir( mProgress->sourceDir.toLocal8Bit().data() );

	/* Perform the IO */
	Q_FOREACH( QString node, sourceList ) {
		if ( mCanceled ) {
			emit canceled( errorNodes );

			quit();
			return;
		}

		while ( isPaused ) {
			if ( wasCanceled ){
				emit canceled( errorNodes );

				quit();
				return;
			}

			usleep( 100 );
			qApp->processEvents();
		}

		struct stat st;
		if ( stat( node.toLocal8Bit().data(), &st ) != 0 )
			errorNodes << node;

		/* Various cases for various types of nodes */
		switch( st.st_mode & S_IFMT ) {

			case S_ISREG: {

				/* Copy a regular file */
				copyFile( node );
				break;
			}

			case S_IFLNK: {

				/* Create a symbolic link */
				symlink( readLink( path + entry->d_name ), ( targetDir + path + entry->d_name ).toLocal8Bit().data() );
				if ( not exists( targetDir + path + entry->d_name ) ) {
					qDebug() << "Error creating symlink" << path + entry->d_name << "->" << readLink( path + entry->d_name );
					errorNodes << path + entry->d_name;
				}
			}

			case S_ISBLK:
			case S_ISCHR:
			case S_IFIFO: {
				/* Create a block device, character special, fifo */

				mknod( targetDir + path + entry->d_name, st.st_mode, st.st_dev );
				break;
			}

			case S_IFSOCK: {

				qDebug() << "Cannot copy a socket:" << path + entry->d_name;
				errorNodes << path + entry->d_name;
				break;
			}
		}
	}

	/* Change back to the current working directory */
	chdir( curWD.toLocal8Bit().data() );

	emit completed( errorNodes );
	mProgress->state = NBProcess::Completed;

	quit();
};

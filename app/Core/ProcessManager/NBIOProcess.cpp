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

#include "NBIOProcess.hpp"

NBIOProcess::NBIOProcess( QStringList sources, NBProcess::Progress *progress ) {

	origSources.clear();
	origSources << sources;

	sourceList.clear();

	mProgress = progress;

	mPaused = false;
	mCanceled = false;
	mUndo = false;

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
	}

	/* We moved the files, move them back */
	else {
		mProgress->sourceDir.swap( mProgress->targetDir );
		start();
	}
};

bool NBIOProcess::preIO() {

	mProgress->state = NBProcess::Starting;

	if ( not isWritable( mProgress->targetDir ) ) {
		emit noWriteAccess();
		mProgress->state = NBProcess::Completed;

		return false;
	}

	/* If we are moving file and in the same partition, we can skip the processing */
	if ( mProgress->type == NBProcess::Move ) {

		struct stat srcStat, tgtStat;
		stat( mProgress->sourceDir.toLocal8Bit().data(), &srcStat );
		stat( mProgress->targetDir.toLocal8Bit().data(), &tgtStat );

		/* If the source and the are the same */
		if ( srcStat.st_dev == tgtStat.st_dev ) {
			/* Add the original sources to the list */
			sourceList << origSources;

			/* Move is just a trivial rename operation, we can get out of here */
			return true;
		}

		/* Otherwise we proceed with the source processing */
	}

	sourceList.clear();

	/* Obtain the file sizes */
	Q_FOREACH( QString src, origSources ) {
		mProgress->progressText = QString( "Processing %1..." ).arg( src );
		if ( isDir( mProgress->sourceDir + src ) )
			processDirectory( src );

		else {

			sourceList << src;
			mProgress->totalBytes += getSize( mProgress->sourceDir + src );
		}
	}

	/* Check if we have enough space to perform the IO */
	mProgress->progressText = QString( "Checking space requirements..." );
	NBDeviceInfo tgtInfo = NBDeviceManager::deviceInfoForPath( mProgress->targetDir );

	if ( tgtInfo.availSpace() <= mProgress->totalBytes ) {

		emit noSpace();

		mProgress->state = NBProcess::Completed;
		return false;
	}

	return true;
};

void NBIOProcess::processDirectory( QString path ) {

	DIR* d_fh;
	struct dirent* entry;

	while ( ( d_fh = opendir( ( mProgress->sourceDir + path ).toLocal8Bit().data() ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	quint64 size = 0;

	if ( not path.endsWith( "/" ) )
		path += "/";

	/* Create this path at the target */
	mkpath( mProgress->targetDir + path );

	/* Now, we can read what is inside this directory */
	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/* Stat the directory to get the mode */
				struct stat st;
				stat( ( mProgress->sourceDir + path + entry->d_name ).toLocal8Bit().data(), &st );

				/* Create this directory at the target */
				mkpath( mProgress->targetDir + path + entry->d_name, st.st_mode );

				/* Recurse into that folder */
				processDirectory( path + entry->d_name );
			}

			else {

				/* Get the size of the current file */
				mProgress->totalBytes += getSize( mProgress->sourceDir + path + entry->d_name );

				/* Add this to the source file list */
				sourceList << path + entry->d_name;
			}
		}
	}

	closedir( d_fh );
};

void NBIOProcess::copyFile( QString srcFile ) {

	QThread::setPriority( QThread::LowestPriority );

	char buffer[ BUFSIZ ];

	qint64 inBytes = 0;
	qint64 bytesWritten = 0;

	QString currentFile = mProgress->targetDir + srcFile;

	if ( not isReadable( mProgress->sourceDir + srcFile ) ) {
		qDebug() << "Unreadable file:" << srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( currentFile ) ) ) {
		qDebug() << currentFile << "not writable!!!";
		errorNodes << srcFile;
		return;
	}

	/* If the file exists, add 'Copy - ' to the beginning of the file name */
	if ( exists( currentFile ) )
		currentFile = dirName( currentFile ) + "/Copy - " + baseName( currentFile );

	struct stat iStat, oStat;
	stat( ( mProgress->sourceDir + srcFile ).toLocal8Bit().data(), &iStat );
	stat( mProgress->targetDir.toLocal8Bit().data(), &oStat );

	/* Open the input file descriptor fro reading */
	int iFileFD = open( ( mProgress->sourceDir + srcFile ).toLocal8Bit().data(), O_RDONLY );

	/* Open the output file descriptor for reading */
	int oFileFD = open( currentFile.toLocal8Bit().data(), O_WRONLY | O_CREAT, iStat.st_mode );

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

		bytesWritten = write( oFileFD, buffer, inBytes );

		if ( bytesWritten != inBytes ) {
			qDebug() << "Error writing to file:" << currentFile;
			qDebug() << "[Error]:" << strerror( errno );
			errorNodes << srcFile;
			break;
		}

		mProgress->fileBytesCopied += bytesWritten;
		mProgress->totalBytesCopied += bytesWritten;
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
};

void NBIOProcess::run() {

	if ( mCanceled ) {
		emit canceled( errorNodes );

		quit();
		return;
	}

	while ( mPaused ) {
		if ( mCanceled ) {
			emit canceled( errorNodes );

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

	/* Actual IO Begins */

	mProgress->progressText = QString();
	mProgress->state = NBProcess::Started;

	if ( mProgress->type == NBProcess::Move ) {
		struct stat srcStat, tgtStat;
		stat( mProgress->sourceDir.toLocal8Bit().data(), &srcStat );
		stat( mProgress->targetDir.toLocal8Bit().data(), &tgtStat );

		/* If the source and the target devices are the same */
		if ( srcStat.st_dev == tgtStat.st_dev ) {
			Q_FOREACH( QString node, sourceList ) {
				QString srcNode = mProgress->sourceDir + node;
				QString tgtNode = mProgress->targetDir + node;
				if ( rename( srcNode.toLocal8Bit().data(), tgtNode.toLocal8Bit().data() ) != 0 ) {
					qDebug() << "Error moving:" << node;
					qDebug() << "[Error]:" << strerror( errno );

					errorNodes << node;
				}
			}

			emit completed( errorNodes );
			mProgress->state = NBProcess::Completed;

			quit();
			return;
		}

		/* Otherwise, we let the copying take place, then delete the sources at the end. */
	}

	/* Copying: Perform the IO */
	Q_FOREACH( QString node, sourceList ) {

		/* Update the current file */
		mProgress->currentFile = node;

		if ( mCanceled ) {
			emit canceled( errorNodes );

			quit();
			return;
		}

		while ( mPaused ) {
			if ( mCanceled ){
				emit canceled( errorNodes );

				quit();
				return;
			}

			usleep( 100 );
			qApp->processEvents();
		}

		struct stat st;
		if ( stat( ( mProgress->sourceDir + node ).toLocal8Bit().data(), &st ) != 0 ) {
			qDebug() << "Stat failed" << node;
			qDebug() << "[Error]:" << strerror( errno );
			errorNodes << node;
		}

		/* Various cases for various types of nodes */
		switch( st.st_mode & S_IFMT ) {

			case S_IFREG: {

				/* Copy a regular file */
				copyFile( node );
				break;
			}

			case S_IFLNK: {

				/* Create a symbolic link */
				symlink( readLink( node ).toLocal8Bit().data(), ( mProgress->targetDir + node ).toLocal8Bit().data() );
				if ( not exists( ( mProgress->targetDir + node ) ) ) {
					qDebug() << "Error creating symlink" << node << "->" << readLink( node );
					qDebug() << "[Error]:" << strerror( errno );
					errorNodes << node;
				}
			}

			case S_IFBLK:
			case S_IFCHR:
			case S_IFIFO: {

				/* Create a block device, character special, fifo */
				mknod( ( mProgress->targetDir + node ).toLocal8Bit().data(), st.st_mode, st.st_dev );
				break;
			}

			case S_IFSOCK: {

				qDebug() << "Cannot copy a socket:" << node;
				errorNodes << node;
				break;
			}
		}
	}

	if ( mProgress->type == NBProcess::Move ) {
		Q_FOREACH( QString node, sourceList ) {
			/* If the source was not copied properly */
			if ( errorNodes.contains( node ) )
				continue;

			/* sourceList will be just a list of files */
			if ( unlink( ( mProgress->sourceDir + node ).toLocal8Bit().data() ) != 0 ) {
				qDebug() << "Error removing original file:" << mProgress->sourceDir + node;
				qDebug() << "[Error]:" << strerror( errno );
				errorNodes << node;
			}
		}

		/* Mixture of files and folders */
		Q_FOREACH( QString node, origSources ) {
			/* Deletion of folders */
			if ( isDir( mProgress->sourceDir + node ) ) {
				/* If a file in this directory was not copied, do not delete it */
				if ( errorNodes.filter( node ).count() )
					continue;

				if ( not removeDir( mProgress->sourceDir + node ) ) {
					qDebug() << "Error removing original directory:" << mProgress->sourceDir + node;
					qDebug() << "[Error]:" << strerror( errno );
					errorNodes << node;
				}
			}

			/* Deletion of files */
			else if ( unlink( ( mProgress->sourceDir + node ).toLocal8Bit().data() ) != 0 ) {
				qDebug() << "Error removing original file:" << mProgress->sourceDir + node;
				qDebug() << "[Error]:" << strerror( errno );
				errorNodes << node;
			}
		}
	}

	emit completed( errorNodes );
	mProgress->state = NBProcess::Completed;

	quit();
};

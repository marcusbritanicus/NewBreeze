/*
	*
	* NBIOProcess.cpp - NewBreeze Process Class for file I/O
	*
	* These classes were originally written by marcusbritanicus@gmail.com
	* for CoreApps LibCPrime
	* This is the heart of copy/move operations.
	*
	* NBProcess::Process is defined in the header NBAbstractProcess.hpp
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
#include "NBMessageDialog.hpp"

NBIOProcess::NBIOProcess( QStringList sources, NBProcess::Process *progress, QWidget* ) : NBAbstractProcess() {

	origSources.clear();
	origSources << sources;

	sourceList.clear();

	mProcess = progress;

	mPaused = false;
	mCanceled = false;

	if ( not mProcess->sourceDir.endsWith( "/" ) )
		mProcess->sourceDir+= "/";

	if ( not mProcess->targetDir.endsWith( "/" ) )
		mProcess->targetDir+= "/";

	/* Initialize the sizes to zero */
	mProcess->totalBytes = 0;
	mProcess->totalBytesCopied = 0;
	mProcess->fileBytes = 0;
	mProcess->fileBytesCopied = 0;

	/* Initialize the state to NBProcess::NotStarted */
	mProcess->state = NBProcess::NotStarted;
};

QStringList NBIOProcess::errors() {

	return errorNodes;
};

void NBIOProcess::cancel() {

	mProcess->state = NBProcess::Canceled;
	mCanceled = true;

	emit canceled( errorNodes );
};

void NBIOProcess::pause() {

	mProcess->state = NBProcess::Paused;
	mPaused = true;
};

void NBIOProcess::resume() {

	mProcess->state = NBProcess::Started;
	mPaused = false;
};

bool NBIOProcess::preIO() {

	mProcess->state = NBProcess::Starting;

	if ( not isWritable( mProcess->targetDir ) ) {
		emit noWriteAccess();
		mProcess->state = NBProcess::Completed;

		return false;
	}

	/* No cut/copy-paste in the same directory */
	if ( mProcess->sourceDir == mProcess->targetDir ) {

		NBMessageDialog::information( nullptr, "Moving finished", "Since the source and target directories are the same, the IO will not proceed." );

		mProcess->state = NBProcess::Completed;
		emit completed( sourceList );

		return false;
	}

	/* If we are moving file and in the same partition, we can skip the processing */
	if ( mProcess->type == NBProcess::Move ) {

		struct stat srcStat, tgtStat;
		stat( mProcess->sourceDir.toLocal8Bit().data(), &srcStat );
		stat( mProcess->targetDir.toLocal8Bit().data(), &tgtStat );

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
		mProcess->progressText = QString( "Processing %1..." ).arg( src );
		if ( isDir( mProcess->sourceDir + src ) )
			processDirectory( src );

		else {

			sourceList << src;
			mProcess->totalBytes += getSize( mProcess->sourceDir + src );
		}
	}

	/* Check if we have enough space to perform the IO */
	mProcess->progressText = QString( "Checking space requirements..." );
	QStorageInfo tgtInfo( mProcess->targetDir );

	if ( tgtInfo.bytesAvailable() <= qint64( mProcess->totalBytes ) ) {

		emit noSpace();

		NBMessageDialog::information( nullptr, "Moving failed", "Not enough space in the target directory." );

		emit completed( sourceList );

		mProcess->state = NBProcess::Completed;
		return false;
	}

	return true;
};

void NBIOProcess::processDirectory( QString path ) {

	DIR* d_fh;
	struct dirent* entry;

    while ( ( d_fh = opendir( ( mProcess->sourceDir + path ).toLocal8Bit().data() ) ) == nullptr ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	if ( not path.endsWith( "/" ) )
		path += "/";

	/* Create this path at the target */
	mkpath( mProcess->targetDir + path, 0755 );

	/* Now, we can read what is inside this directory */
    while( ( entry = readdir( d_fh ) ) != nullptr ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			if ( entry->d_type == DT_DIR ) {

				/* Stat the directory to get the mode */
				struct stat st;
				stat( ( mProcess->sourceDir + path + entry->d_name ).toLocal8Bit().data(), &st );

				/* Create this directory at the target */
				mkpath( mProcess->targetDir + path + entry->d_name, st.st_mode );

				/* Recurse into that folder */
				processDirectory( path + entry->d_name );
			}

			else {

				/* Get the size of the current file */
				mProcess->totalBytes += getSize( mProcess->sourceDir + path + entry->d_name );

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

	QString currentFile = mProcess->targetDir + srcFile;

	if ( not isReadable( mProcess->sourceDir + srcFile ) ) {
		qDebug() << "Unreadable file:" << srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( currentFile ) ) ) {
		qDebug() << currentFile << "not writable!!!";
		errorNodes << srcFile;
		return;
	}

	/* If the file exists, ask the user what is to be done */
	/* 0 - Ask every time or not yet asked */
	/* 1 - Replace all */
	/* 2 - Keep all */
	if ( exists( currentFile ) ) {
		/* Cut-or-Copy/paste in the same folder: Should not come here */
		if ( mProcess->sourceDir == mProcess->targetDir ) {
			resolution = QMessageBox::No;
		}

		/* Unresolved */
		if ( not mResolveConflict ) {
			/* Wait resolution */
			if ( resolution == QMessageBox::NoButton )
				emit resolveConflict( currentFile );

			while ( resolution == QMessageBox::NoButton ) {
				usleep( 100 );
				qApp->processEvents();
			}

			/* Keep existing; mResolveConflict = 0 */
			if ( resolution == QMessageBox::No ) {
				mResolveConflict = 0;
				currentFile = newFileName( currentFile );
			}

			/* Keep all existing; mResolveConflict = 2 */
			else if ( resolution == QMessageBox::NoToAll ) {
				mResolveConflict = 2;
				currentFile = newFileName( currentFile );
			}

			/* Ignore: forget copying the current file; mResolveConflict = 0 */
			else if ( resolution == QMessageBox::Ignore ) {
				mResolveConflict = 0;
				if ( mProcess->type == NBProcess::Move )
					errorNodes <<  srcFile;

				return;
			}

			/* Replace: remove existing (assume we can delete), then copy; mResolveConflict = 0 */
			else if ( resolution == QMessageBox::Yes ) {
				mResolveConflict = 0;
				QFile::remove( currentFile );
			}

			/* Replace all: remove existing (assume we can delete), then copy; mResolveConflict = 1 */
			else if ( resolution == QMessageBox::YesToAll ) {
				mResolveConflict = 1;
				QFile::remove( currentFile );
			}

			else {
				/* Should never come here; but same QMessageBox::Ignore */
				mResolveConflict = 0;
				return;
			}

			resolution = QMessageBox::NoButton;
		}

		/* Replace all: remove existing file */
		else if ( mResolveConflict == 1 )
			QFile::remove( currentFile );

		/* Keep all: Rename current */
		else if ( mResolveConflict == 2 )
			currentFile = newFileName( currentFile );

		/* Ignore: forget the copying */
		else
			return;
	}

	struct stat iStat, oStat;
	stat( ( mProcess->sourceDir + srcFile ).toLocal8Bit().data(), &iStat );
	stat( mProcess->targetDir.toLocal8Bit().data(), &oStat );

	/* Open the input file descriptor fro reading */
	int iFileFD = open( ( mProcess->sourceDir + srcFile ).toLocal8Bit().data(), O_RDONLY );

	/* Open the output file descriptor for reading */
	int oFileFD = open( currentFile.toLocal8Bit().data(), O_WRONLY | O_CREAT, iStat.st_mode );

	/* NBProcess::Process::fileBytes */
	mProcess->fileBytes = iStat.st_size;

	/* NBProcess::Process::fileBytesCopied */
	mProcess->fileBytesCopied = 0;

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

		mProcess->fileBytesCopied += bytesWritten;
		mProcess->totalBytesCopied += bytesWritten;
	}

	close( iFileFD );
	close( oFileFD );

	/* If read(...) resulted in an error */
	if ( inBytes == -1 ) {
		qDebug() << "Error copying file:" << srcFile;
		qDebug() << "[Error]:" << strerror( errno );
		errorNodes << srcFile;
	}

	if ( mProcess->fileBytesCopied != quint64( iStat.st_size ) )
		errorNodes << srcFile;
};

QString NBIOProcess::newFileName( QString fileName ) {

	int i = 0;
	QString newFile;

	do {
		newFile = dirName( fileName ) + QString( "/Copy (%1) - " ).arg( i ) + baseName( fileName );
		i++;
	} while( exists( newFile ) );

	return newFile;
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

		emit completed( QStringList() );

		quit();
		return;
	}

	/* Actual IO Begins */

	mProcess->progressText = QString();
	mProcess->state = NBProcess::Started;

	if ( mProcess->type == NBProcess::Move ) {
		struct stat srcStat, tgtStat;
		stat( mProcess->sourceDir.toLocal8Bit().data(), &srcStat );
		stat( mProcess->targetDir.toLocal8Bit().data(), &tgtStat );

		/* If the source and the target devices are the same */
		QStringList moveList( sourceList );
		if ( srcStat.st_dev == tgtStat.st_dev ) {
			for( int i = 0; i < moveList.count(); i++ ) {
				QString node = moveList.value( i );

				QString srcNode = mProcess->sourceDir + node;
				QString tgtNode = mProcess->targetDir + node;

				if ( not exists( tgtNode ) ) {
					if ( QFile::rename( srcNode, tgtNode ) )
						sourceList.removeAt( i );

					else {
						QString node = sourceList.takeAt( i );
						if ( isDir( srcNode ) )
							processDirectory( node );

						else
							mProcess->totalBytes += getSize( node );
					}
				}

				else {
					QString node = sourceList.takeAt( i );
					if ( isDir( srcNode ) )
						processDirectory( node );

					else
						mProcess->totalBytes += getSize( node );
				}
			}

			/* If all files have been moved, then signal the end */
			if ( not sourceList.count() ) {
				emit completed( errorNodes );
				mProcess->state = NBProcess::Completed;

				quit();
				return;
			}

			/* Some files might not have been moved, try copying them. */
		}

		/* Otherwise, we let the copying take place, then delete the sources at the end. */
	}

	/* Copying: Perform the IO */
	Q_FOREACH( QString node, sourceList ) {

		/* Update the current file */
		mProcess->currentFile = node;

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
		if ( stat( ( mProcess->sourceDir + node ).toLocal8Bit().data(), &st ) != 0 ) {
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
				symlink( readLink( node ).toLocal8Bit().data(), ( mProcess->targetDir + node ).toLocal8Bit().data() );
				if ( not exists( ( mProcess->targetDir + node ) ) ) {
					qDebug() << "Error creating symlink (symlink(...))" << node << "->" << readLink( node );
					qDebug() << "[Error]:" << strerror( errno );
					errorNodes << node;
				}
				break;
			}

			case S_IFBLK:
			case S_IFCHR:
			case S_IFIFO: {

				/* Create a block device, character special, fifo */
				mknod( ( mProcess->targetDir + node ).toLocal8Bit().data(), st.st_mode, st.st_dev );
				break;
			}

			case S_IFSOCK: {

				qDebug() << "Cannot copy a socket:" << node;
				errorNodes << node;
				break;
			}
		}
	}

	if ( mProcess->type == NBProcess::Move ) {
		qDebug() << errorNodes;
		Q_FOREACH( QString node, sourceList ) {
			qDebug() << node << errorNodes.contains( node );
			/* If the source was not copied properly */
			if ( errorNodes.contains( node ) )
				continue;

			/* sourceList will be just a list of files */
			if ( unlink( ( mProcess->sourceDir + node ).toLocal8Bit().data() ) != 0 ) {
				qDebug() << "Error removing original file:" << mProcess->sourceDir + node;
				qDebug() << "[Error]:" << strerror( errno );
			}
		}

		/* Mixture of files and folders */
		Q_FOREACH( QString node, origSources ) {
			/* If the source was not copied properly */
			if ( errorNodes.contains( node ) )
				continue;

			/* Deletion of folders */
			if ( isDir( mProcess->sourceDir + node ) ) {
				/* If a file in this directory was not copied, do not delete it */
				if ( errorNodes.filter( node ).count() )
					continue;

				if ( not removeDir( mProcess->sourceDir + node ) ) {
					qDebug() << "Error removing original directory:" << mProcess->sourceDir + node;
					qDebug() << "[Error]:" << strerror( errno );
				}
			}

			/* Deletion of files */
			else if ( unlink( ( mProcess->sourceDir + node ).toLocal8Bit().data() ) != 0 ) {
				qDebug() << "Error removing original file:" << mProcess->sourceDir + node;
				qDebug() << "[Error]:" << strerror( errno );
			}
		}
	}

	emit completed( errorNodes );
	mProcess->state = NBProcess::Completed;

	quit();
};

QMessageBox::StandardButton ConflictDialog::resolveConflict( QString fileName, QWidget *parent ) {

	QString title = QString( "NewBreeze - File Exists" );
	QString message = QString(
		"<p>The file you are trying to copy</p><center><b>%1</b></center>"
		"<p>already exists in the target directory. What would you like to do?</p>"
		"<tt>[Yes]</tt> - Replace<br>"
		"<tt>[Yes to All]</tt> - Replace all existing files<br>"
		"<tt>[No]</tt> - Keep both files<br>"
		"<tt>[No to All]</tt> - Keep all existing files<br>"
		"<tt>[Ignore]</tt> - Skip copying files if they exist"
	).arg( baseName( fileName ) );

	QMessageBox::StandardButtons buttons = QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No | QMessageBox::NoToAll | QMessageBox::Ignore;

	return QMessageBox::question( parent, title, message, buttons, QMessageBox::No );
};

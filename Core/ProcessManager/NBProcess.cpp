/*
	*
	* NBProcess.cpp - NewBreeze Process Class for I/O, permission settings, etc
	*
*/

#include <NBProcess.hpp>

NBIOProcess::NBIOProcess( QStringList sources, QString target, NBProcess::Progress *progress ) {

	sourceList.clear();
	target.clear();

	sourceList << sources;
	targetDor = target;

	mProgress = progress;
};

void NBIOProcess::preIO() {

	/* Store the current working directory */
	QString curWD = QString::fromLocal8Bit( get_current_dir_name() );

	/* Switch to the source directory */
	chdir( mProgress->sourceDir.toLocal8Bit().data() );

	/* Set the progress text as computing file sizes  */
	mProgress->progressText = QString( "Computing file sizes..." );

	/* Obtain the file sizes */
	Q_FOREACH( QString src, sourceList )
		mProgress->totalBytes += getSize( src );

	/* Change back to the current working directory */
	chdir( qPrintable( curWD ) );
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

	if ( exists( mProgress->currentFile ) ) {
		QString suffix = mimeDb.suffixForFileName( srcFile );
		mProgress->currentFile = ( suffix.length() ? mProgress->currentFile.left( mProgress->currentFile.length() - ( 1 + suffix.length() ) ) + " - Copy." + suffix : mProgress->currentFile + " - Copy" );
	}

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

	/* While */
	while ( ( inBytes = read( iFileFD, buffer, BUFSIZ ) ) > 0 ) {
		if ( mCanceled ) {
			close( iFileFD );
			close( oFileFD );

			return;
		}

		while ( mPaused ) {
			if ( mCanceled ) {
				close( iFileFD );
				close( oFileFD );

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

void NBIOProcess::copyDir( QString path ) {

	/* If the operation is intra-device operation and its a move, then we can simply rename the file */
	struct stat iStat, oStat;

	stat( path.toLocal8Bit().data(), &iStat );
	stat( targetDir.toLocal8Bit().data(), &oStat );

	if ( ( iStat.st_dev == oStat.st_dev ) and ( mProgress->type == NBProcess::Move ) ) {
		/*
			*
			* If the rename fails, then the target may be existing and is not empty
			* In such a case, we must perform a normal copy
			*
		*/
		if ( rename( path.toLocal8Bit().data(), ( targetDir + path ).toLocal8Bit().data() ) == 0 )
			return;
	}

	DIR* d_fh;
	struct dirent* entry;

	if ( wasCanceled )
		return;

	/* Create this path at the target */
	mkpath( targetDir + path );

	while( ( d_fh = opendir( path.toLocal8Bit().data() ) ) == NULL ) {
		qWarning() << "Couldn't open directory:" << path;
		return;
	}

	if ( not path.endsWith( "/" ) )
		path += "/";

	while( ( entry = readdir( d_fh ) ) != NULL ) {

		/* Don't descend up the tree or include the current directory */
		if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

			switch( entry->d_type ) {
				/* If it's a directory print it's name and recurse into it */
				case DT_DIR: {

					/* Prepend the current directory name and recurse */
					copyDir( path + entry->d_name );
					break;
				}

				case DT_REG: {

					/* Copy the current file */
					copyFile( path + entry->d_name );
					break;
				}

				case DT_LNK: {

					/* Create a symbolic link */
					symlink( readLink( path + entry->d_name ), ( targetDir + path + entry->d_name ).toLocal8Bit().data() );
					if ( not exists( targetDir + path + entry->d_name ) ) {
						qDebug() << "Error creating symlink" << path + entry->d_name << "->" << readLink( path + entry->d_name );
						errorNodes << path + entry->d_name;
					}
				}

				case DT_BLK:
				case DT_CHR:
				case DT_BLK: {
					struct stat stbuf;
					if ( stat( path.toLocal8Bit().data(), &stbuf ) == 0 )
						mknod( targetDir + path + entry->d_name, stbuf.st_mode, stbuf.st_dev );

					break;
				}

				case DT_SOCK: {

					qDebug() << "Cannot copy a socket:" << path + entry->d_name;
					errorNodes << path + entry->d_name;
					break;
				}
			}
		}
	}

	closedir( d_fh );

	if ( mProgress->type == NBProcess::Move )
		rmdir( path.toLocal8Bit().data() );
};

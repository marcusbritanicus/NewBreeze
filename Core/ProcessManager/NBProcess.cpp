/*
	*
	* NBProcess.cpp - NewBreeze Process Class for I/O, permission settings, etc
	*
*/

#include <NBProcess.hpp>

NBIOProcess::NBIOProcess( QStringList sources, QString target, NBProcess::Type type, NBProcess::Progress *progress ) {

};

void NBIOProcess::copyFile( QString srcFile ) {

	char buffer[ BUFSIZ ];

	qint64 inBytes = 0;
	qint64 bytesWritten = 0;

	mProgress->currentFile = targetDir + srcFile;

	if ( not isReadable( srcFile ) ) {
		qDebug() << "Unreadable file: " + srcFile;
		errorNodes << srcFile;
		return;
	}

	if ( not isWritable( dirName( mProgress->currentFile ) ) ) {
		qDebug() << mProgress->currentFile + " not writable!!!";
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
	if ( ( iStat.st_dev == oStat.st_dev ) and ( NBIOMode::Move == mode ) ) {
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

	if ( mode == NBProcess::Move )
		/* If there were no errors encountered, and its a move operation, delete the file */
		if ( not errorNodes.contains( srcFile ) )
			unlink( srcFile.toLocal8Bit().data() );
};

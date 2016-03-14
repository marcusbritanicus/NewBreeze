/*
	*
	* NBDeleteProcess.cpp - NewBreeze Process Class for file I/O
	*
*/

#include <NBDeleteProcess.hpp>
#include <NBTools.hpp>
#include <NBXdg.hpp>

NBDeleteProcess::NBDeleteProcess( QStringList sources, NBProcess::Progress *progress ) {

	/* Switch off flags */
	mCanceled = false;
	mPaused = false;
	mUndo = false;

	/* Set the source list */
	sourceList.clear();
	sourceList << sources;

	/* Make a local copy of the progress pointer */
	mProgress = progress;

	if ( not mProgress->sourceDir.endsWith( "/" ) )
		mProgress->sourceDir+= "/";

	/* Initialize the sizes to zero */
	mProgress->totalBytes = 1;
	mProgress->totalBytesCopied = -1;
	mProgress->fileBytes = 1;
	mProgress->fileBytesCopied = -1;

	/* Initialize the state to NBProcess::NotStarted */
	mProgress->state = NBProcess::NotStarted;
};

QStringList NBDeleteProcess::errors() {

	return errorNodes;
};

void NBDeleteProcess::cancel() {

	mProgress->state = NBProcess::Canceled;
	mCanceled = true;
};

void NBDeleteProcess::pause() {

	mProgress->state = NBProcess::Paused;
	mPaused = true;
};

void NBDeleteProcess::resume() {

	mProgress->state = NBProcess::Started;
	mPaused = false;
};

bool NBDeleteProcess::canUndo() {

	return ( mProgress->type != NBProcess::Delete );
};

void NBDeleteProcess::undo() {

	if ( mProgress->type == NBProcess::Trash ) {
		mUndo = true;
		start();
	}
};

void NBDeleteProcess::deleteNode( QString path ) {

	if ( mCanceled ) {
		emit canceled( errorNodes );

		quit();
		return;
	}

	while ( mPaused ) {
		if ( mCanceled ){
			emit canceled( errorNodes );

			return;
		}

		usleep( 100 );
		qApp->processEvents();
	}

	path = mProgress->sourceDir + path;

	struct stat st;
	if ( stat( path.toLocal8Bit().data(), &st ) != 0 ) {
		qDebug() << "Unable to stat:" << path;
		errorNodes << path;

		return;
	}

	switch( st.st_mode && S_IFMT ) {
		case S_IFDIR: {

			DIR* d_fh;
			struct dirent* entry;

			while ( ( d_fh = opendir( path.toLocal8Bit().data() ) ) == NULL ) {
				qWarning() << "Couldn't open directory:" << path;
				errorNodes << path;
				return;
			}

			if ( not path.endsWith( "/" ) )
				path += "/";

			while( ( entry = readdir( d_fh ) ) != NULL ) {

				/* Don't descend up the tree or include the current directory */
				if ( strcmp( entry->d_name, ".." ) != 0 && strcmp( entry->d_name, "." ) != 0 ) {

					if ( entry->d_type == DT_DIR ) {

						/* Recurse into that folder */
						deleteNode( path + entry->d_name );
					}

					else {
						if ( unlink( ( path + entry->d_name ).toLocal8Bit().data() ) != 0 )
							errorNodes << path + entry->d_name;
					}
				}
			}

			if ( unlink( path.toLocal8Bit().data() ) != 0 )
				errorNodes << path;

			closedir( d_fh );
			break;
		}

		default: {
			if ( unlink( path.toLocal8Bit().data() ) != 0 )
				errorNodes << path;

			break;
		}
	}
};

void NBDeleteProcess::trashNode( QString node ) {

	if ( mCanceled ) {
		emit canceled( errorNodes );

		quit();
		return;
	}

	while ( mPaused ) {
		if ( mCanceled ){
			emit canceled( errorNodes );

			return;
		}

		usleep( 100 );
		qApp->processEvents();
	}

	QString trashLoc = NBXdg::trashLocation( mProgress->sourceDir );

	QString newPath = trashLoc + "/files/" + baseName( node );
	QString delTime = QDateTime::currentDateTime().toString( "yyyyMMddThh:mm:ss" );

	/* If it exists, add a date time to it to make it unique */
	if ( access( newPath.toLocal8Bit().data(), R_OK ) == 0 )
		newPath += delTime;

	/* Try trashing it. If it fails, intimate the user */
	if ( rename( ( mProgress->sourceDir + node ).toLocal8Bit().data(), newPath.toLocal8Bit().data() ) ) {
		qDebug() << "Error" << errno << ": Failed to trash " << node << ":" << strerror( errno );
		errorNodes << node;
	}

	/* If it succeeds, we write the meta data */
	else {
		QFile metadata( trashLoc + "/info/" + baseName( newPath ) + ".trashinfo" );
		metadata.open( QIODevice::WriteOnly );
		metadata.write(
			QString(
				"[Trash Info]\n"
				"Path=%1\n"
				"DeletionDate=%2\n"
			).arg( node ).arg( delTime ).toLocal8Bit()
		);
		metadata.close();

		/* An ugly hack: Shortcut for TrashModel listing */
		QSettings trashInfo( "NewBreeze", "TrashInfo" );
		trashInfo.setValue( baseName( node ), QStringList() << node << delTime << newPath );
	}
};

void NBDeleteProcess::restoreNode( QString path ) {

	Q_UNUSED( path );
};

void NBDeleteProcess::run() {

	/* Undo for NBProcess::Copy */
	if ( mUndo ) {

		mProgress->state = NBProcess::Started;

		if ( mCanceled ) {
			emit canceled( errorNodes );

			quit();
			return;
		}

		while ( mPaused ) {
			if ( mCanceled ){
				emit canceled( errorNodes );

				return;
			}

			usleep( 100 );
			qApp->processEvents();
		}

		errorNodes.clear();

		/* We want the progressbars to be swaying */
		mProgress->totalBytesCopied = -1;
		mProgress->fileBytesCopied = -1;

		QString trashLoc = NBXdg::trashLocation( mProgress->sourceDir );

		Q_FOREACH( QString path, sourceList ) {
			if ( mCanceled ) {
				emit canceled( errorNodes );

				quit();
				return;
			}

			while ( mPaused ) {
				if ( mCanceled ){
					emit canceled( errorNodes );

					return;
				}

				usleep( 100 );
				qApp->processEvents();
			}

			if ( errorNodes.contains( path ) )
				continue;

			restoreNode( path );
		}

		mUndo = false;

		mProgress->state = NBProcess::Completed;
		emit completed( errorNodes );

		quit();
		return;
	}

	/* Actual Deletion/Trashing */
	if ( mCanceled ) {
		emit canceled( errorNodes );

		quit();
		return;
	}

	while ( mPaused ) {
		if ( mCanceled ){
			emit canceled( errorNodes );

			return;
		}

		usleep( 100 );
		qApp->processEvents();
	}

	mProgress->progressText = QString();
	mProgress->state = NBProcess::Started;

	Q_FOREACH( QString path, sourceList ) {
		if ( mProgress->type == NBProcess::Delete )
			deleteNode( path );

		else
			trashNode( path );
	}

	emit completed( errorNodes );
	mProgress->state = NBProcess::Completed;

	quit();
};

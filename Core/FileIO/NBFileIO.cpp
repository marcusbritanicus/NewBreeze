/*
	*
	* NBFileIOManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>
#include <NBTools.hpp>
#include <NBDeleteManager.hpp>

NBFileIO::NBFileIO( quint64 jobNum ) : QThread(), m_jobID( jobNum ) {

	totalBytesToBeCopied = 0;
	totalBytesCopied = 0;

	paused = false;
	canceled = false;

	/* We give a progress update every 500 ms */
	timer = new QTimer();
	timer->setInterval( 500 );

	connect( timer, SIGNAL( timeout() ), this, SLOT( progressClick() ) );
};

void NBFileIO::setMode( NBIOMode::Mode mode ) {

	m_mode = mode;
};

void NBFileIO::setSources( QStringList srcList ) {

	m_sourceList << srcList;
};

void NBFileIO::setTarget( QString tgt ) {

	m_target = QString( tgt );
};

quint64 NBFileIO::jobID() {

	return m_jobID;
};

void NBFileIO::run() {

	// Compute IO amount: totalBytes, totalFiles etc
	quint64 files = 0;
	foreach( QString path, m_sourceList ) {
		QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
		while ( it.hasNext() ) {
			if( canceled )
				return;

			it.next();
			if( it.fileInfo().isDir() ) {
				if ( it.filePath() == path )
					continue;
			}

			else {
				files++;
				totalBytesToBeCopied += it.fileInfo().size();
			}
		}
	}
	emit ioVolume( m_jobID, files, totalBytesToBeCopied );

	if ( m_mode == NBIOMode::Copy )
		copyItems( m_sourceList, m_target );

	else
		moveItems( m_sourceList, m_target );

	emit complete( m_jobID, failedNodes );
	exec();
};

void NBFileIO::cancelIO() {

	canceled = true;
};

float NBFileIO::progress() {

	if ( totalBytesToBeCopied )
		return totalBytesCopied / totalBytesToBeCopied;

	else
		return 0;
};

void NBFileIO::pause() {

	paused = true;
};

void NBFileIO::resume() {

	paused = false;
};

void NBFileIO::copyItems( QStringList srcList, QString tgt ) {

	timer->start();
	foreach( QString src, srcList ) {
		/*
			*
			* If the files from the current folder are being pasted into the same folder
			* then we need to make a copy
			*

			*
			* if @src is a file, then target becomes target + Copy of baseName( @src )
			* if @src is a directory, then files from @src are copied into target + Copy of baseName( @src )
			*
			* baseName : function defined in NBTools.hpp
			*
		*/

		/*
			*
			* Every we start with a new node copying, we emit this signal
			* Based on this signal, NBFileSystemModel will be updated.
			*
		*/

		if ( dirName( src ) == tgt ) {
			QString source = src;
			QString target = QDir( tgt ).filePath( "Copy of " + baseName( src ) );

			if ( isFile( src ) )
				copyFile( source, target );

			else if ( isDir( src ) )
				copyDir( source, target );

			else
				failedNodes << source;
		}

		/*
			*
			* The files and folder to be pasted do not originate from this directory.
			* But check for the existence of similarly named files/folders.
			* If similarly named files/folders exist, then we follow the above procedure.
			*
		*/
		else if ( exists( QDir( tgt ).filePath( baseName( src ) ) ) ) {
			QString source = src;
			QString target = QDir( tgt ).filePath( "Copy of " + baseName( src ) );

			if ( isFile( src ) )
				copyFile( source, target );

			else if ( isDir( src ) )
				copyDir( source, target );

			else
				failedNodes << source;
		}

		/*
			*
			* Now we are sure the source and target are different
			* If source is a file, copy it directly
			*
		*/

		else {
			QString source = src;
			QString target = QDir( tgt ).filePath( baseName( src ) );

			if ( isFile( src ) )
				copyFile( source, target );

			else if ( isDir( src ) )
				copyDir( source, target );

			else
				failedNodes << src;
		}
	}
	timer->stop();
};

void NBFileIO::moveItems( QStringList srcList, QString tgt ) {

	NBDeleteManager *deleter = new NBDeleteManager( this, false );

	timer->start();
	foreach( QString src, srcList ) {
		/*
			*
			* If the files from the current folder are being pasted into the same folder
			* then we have a problem
			*
		*/
		if ( dirName( src ) == tgt ) {
			failedNodes << src;
		}

		/*
			*
			* The files and folder to be pasted do not originate from this directory.
			* But check for the existence of similarly named files/folders.
			* If similarly named files/folders exist, then we follow the above procedure.
			*
		*/
		else if ( exists( QDir( tgt ).filePath( baseName( src ) ) ) ) {
			QString source = src;
			QString target = QDir( tgt ).filePath( "Copy of " + baseName( src ) );

			if ( isFile( src ) ) {
				if ( copyFile( source, target ) )
					QFile::remove( source );
			}

			else if ( isDir( src ) ) {
				if ( copyDir( source, target ) )
					deleter->deleteFromDisk( QStringList() << source );
			}

			else {
				failedNodes << source;
			}
		}

		/*
			*
			* Now we are sure the source and target are different
			* If source is a file, copy it directly
			*
		*/

		else {
			QString source = src;
			QString target = QDir( tgt ).filePath( baseName( src ) );

			if ( isFile( src ) ) {
				if ( copyFile( source, target ) )
					QFile::remove( source );
			}

			else if ( isDir( src ) ) {
				if ( copyDir( source, target ) )
					deleter->deleteFromDisk( QStringList() << source );
			}

			else {
				failedNodes << source;
			}
		}
	}
	timer->stop();
};

bool NBFileIO::copyDir( QString srcDir, QString tgtDir ) {
	/*
		*
		* All nodes in srcDir are to be copied to tgtDir
		* If tgtDir does not exist, create it
		*
	*/

	bool success = true;

	if ( not exists( tgtDir ) )
		QDir( "." ).mkpath( tgtDir );

	foreach( QFileInfo info, QDir( srcDir ).entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst ) ) {
		QString source = info.absoluteFilePath();
		QString target = QDir( tgtDir ).filePath( baseName( source ) ) ;

		if ( info.isDir() )
			success &= copyDir( source, target );

		else
			success &= copyFile( source, target );
	}

	return success;
};

bool NBFileIO::copyFile( QString srcFile, QString tgtFile ) {

	QFile iFile( srcFile );
	QFile oFile( tgtFile );

	m_tgtFile = QString( tgtFile );

	cfileBytesToBeCopied = iFile.size();
	cfileBytesCopied = 0;

	if ( not iFile.open( QFile::ReadOnly ) ) {
		failedNodes << srcFile;
		return false;
	}

	if ( not oFile.open( QFile::WriteOnly ) ) {
		failedNodes << srcFile;
		return false;
	}

	while( not iFile.atEnd() ) {
		if ( canceled ) {
			iFile.close();
			oFile.close();

			return false;
		}

		while ( paused )
			continue;

		char block[ 4096 ] = { 0 };
		qint64 inBytes = iFile.read( block, sizeof( block ) );
		oFile.write( block, inBytes );
		totalBytesCopied += inBytes;
		cfileBytesCopied += inBytes;
	}

	iFile.close();
	oFile.close();

	oFile.setPermissions( iFile.permissions() );
	progressClick();

	return true;
};

void NBFileIO::start() {

	QThread::start( QThread::InheritPriority );
};

void NBFileIO::progressClick() {

	float tbcF = 0, fbcF = 0;
	if ( totalBytesToBeCopied )
		tbcF = totalBytesCopied / totalBytesToBeCopied;

	if ( cfileBytesToBeCopied )
		fbcF = cfileBytesCopied / cfileBytesToBeCopied;

	emit copied( m_jobID, m_tgtFile, fbcF, tbcF );
};

/*
	*
	* NBFileIOManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>
#include <NBTools.hpp>
#include <NBDeleteManager.hpp>

NBFileIO::NBFileIO() : QThread() {

	nodes = 0;
	totalBytesToBeCopied = 0;
	totalBytesCopied = 0;

	paused = false;
	canceled = false;
};

void NBFileIO::setMode( NBIOMode::Mode mode ) {

	m_mode = mode;
};

void NBFileIO::setSources( QStringList srcList ) {

	m_sourceList << srcList;
};

QStringList NBFileIO::sources() {

	return m_sourceList;
};

void NBFileIO::setTarget( QString target ) {

	m_target = QString( target );
};

QString NBFileIO::target() {

	return m_target;
};

void NBFileIO::setJobID( QString jobID ) {

	m_jobID = QString( jobID );
};

QString NBFileIO::jobID() {

	return m_jobID;
};

void NBFileIO::run() {

	// Compute IO amount: totalBytes, totalnodes etc
	quint64 nodes = 0;
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
				nodes++;
				totalBytesToBeCopied += it.fileInfo().size();
			}
		}
	}

	if ( m_mode == NBIOMode::Copy ) {
		foreach( QString src, m_sourceList ) {
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

			if ( dirName( src ) == m_target ) {
				QString source = src;
				QString target = QDir( m_target ).filePath( "Copy of " + baseName( src ) );

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
			else if ( exists( QDir( m_target ).filePath( baseName( src ) ) ) ) {
				QString source = src;
				QString target = QDir( m_target ).filePath( "Copy of " + baseName( src ) );

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
				QString target = QDir( m_target ).filePath( baseName( src ) );

				if ( isFile( src ) )
					copyFile( source, target );

				else if ( isDir( src ) )
					copyDir( source, target );

				else
					failedNodes << src;
			}
		}
	}

	else {
		NBDeleteManager *deleter = new NBDeleteManager( this, false );

		foreach( QString src, m_sourceList ) {
			/*
				*
				* If the files from the current folder are being pasted into the same folder
				* then we have a problem
				*
			*/
			if ( dirName( src ) == m_target ) {
				failedNodes << src;
			}

			/*
				*
				* The files and folder to be pasted do not originate from this directory.
				* But check for the existence of similarly named files/folders.
				* If similarly named files/folders exist, then we follow the above procedure.
				*
			*/
			else if ( exists( QDir( m_target ).filePath( baseName( src ) ) ) ) {
				QString source = src;
				QString target = QDir( m_target ).filePath( "Copy of " + baseName( src ) );

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
				QString target = QDir( m_target ).filePath( baseName( src ) );

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
	}

	emit complete( m_jobID, failedNodes );
};

void NBFileIO::cancelIO() {

	canceled = true;
};

bool NBFileIO::wait( int msec ) {

	QTimer timer;
	QEventLoop loop;

	connect( &timer, SIGNAL( timeout() ), &loop, SLOT( quit() ) );
	connect( this, SIGNAL( complete( QString, QStringList ) ), &loop, SLOT( quit() ) );

	timer.start( msec );
	loop.exec();

	if ( timer.isActive() )
		return true;

	else
		return false;
};

quint64 NBFileIO::totalNodes() {

	return nodes;
};

quint64 NBFileIO::totalBytes() {

	return totalBytesToBeCopied;
};

QString NBFileIO::currentFileCopied() {

	return m_tgtFile;
};

quint64 NBFileIO::copiedBytes() {

	return totalBytesCopied;
};

quint64 NBFileIO::cfileTotalBytes() {

	return cfileBytesToBeCopied;
};

quint64 NBFileIO::cfileCopiedBytes() {

	return cfileBytesCopied;
};

float NBFileIO::totalProgress() {

	if ( totalBytesToBeCopied )
		return totalBytesCopied / totalBytesToBeCopied;

	else
		return 0;
};

float NBFileIO::cfileProgress() {

	if ( cfileBytesToBeCopied )
		return cfileBytesCopied / cfileBytesToBeCopied;

	else
		return 0;
};

void NBFileIO::pause() {

	paused = true;
};

void NBFileIO::resume() {

	paused = false;
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

	return true;
};

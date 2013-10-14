/*
	*
	* NBFileIOManager.cpp - NewBreeze FileIO Manager
	*
*/

#include <NBFileIO.hpp>
#include <NBTools.hpp>
#include <NBDeleteManager.hpp>

bool copyFile( Job *job, QString srcFile, QString tgtFile ) {

	QFile iFile( srcFile );
	QFile oFile( tgtFile );

	job->cfileBytesCopied = 0;
	job->currentFile = QString( tgtFile );
	job->cfileBytes = getSize( srcFile );

	if ( not iFile.open( QFile::ReadOnly ) ) {
		job->errorNodes << srcFile;
		return false;
	}

	if ( not oFile.open( QFile::WriteOnly ) ) {
		job->errorNodes << srcFile;
		return false;
	}

	while( not iFile.atEnd() ) {
		if ( job->canceled ) {
			job->errorNodes << srcFile;

			iFile.close();
			oFile.close();

			return false;
		}

		while ( job->paused )
			continue;

		char block[ 4096 ] = { 0 };
		qint64 inBytes = iFile.read( block, sizeof( block ) );
		oFile.write( block, inBytes );
		job->totalBytesCopied += inBytes;
		job->cfileBytesCopied += inBytes;
	}

	iFile.close();
	oFile.close();

	oFile.setPermissions( iFile.permissions() );

	return true;
};

bool copyDir( Job *job, QString srcDir, QString tgtDir ) {
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
		if ( job->canceled ) {
			job->errorNodes << info.absoluteFilePath();
			success &= false;
			continue;
		}

		QString source = info.absoluteFilePath();
		QString target = QDir( tgtDir ).filePath( baseName( source ) ) ;

		if ( info.isDir() )
			success &= copyDir( job, source, target );

		else
			success &= copyFile( job, source, target );
	}

	return success;
};

void performIO( Job *job ) {

	job->running = true;

	// Compute IO amount: totalBytes, totalnodes etc
	foreach( QString path, job->sources ) {
		QDirIterator it( path, QDir::AllEntries | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Hidden, QDirIterator::Subdirectories );
		while ( it.hasNext() ) {
			if( job->canceled )
				return;

			it.next();
			if( it.fileInfo().isDir() ) {
				if ( it.filePath() == path )
					continue;
			}

			else {
				job->totalNodes++;
				job->totalBytes += it.fileInfo().size();
			}
		}
	}

	// We have a Copy Operation to do
	if ( job->mode == NBIOMode::Copy ) {
		foreach( QString src, job->sources ) {
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

			if ( dirName( src ) == job->target ) {
				QString source = src;
				QString target = QDir( job->target ).filePath( "Copy of " + baseName( src ) );

				if ( isFile( src ) )
					copyFile( job, source, target );

				else if ( isDir( src ) )
					copyDir( job, source, target );

				else
					job->errorNodes << source;
			}

			/*
				*
				* The files and folder to be pasted do not originate from this directory.
				* But check for the existence of similarly named files/folders.
				* If similarly named files/folders exist, then we follow the above procedure.
				*
			*/
			else if ( exists( QDir( job->target ).filePath( baseName( src ) ) ) ) {
				QString source = src;
				QString target = QDir( job->target ).filePath( "Copy of " + baseName( src ) );

				if ( isFile( src ) )
					copyFile( job, source, target );

				else if ( isDir( src ) )
					copyDir( job, source, target );

				else
					job->errorNodes << source;
			}

			/*
				*
				* Now we are sure the source and target are different
				* If source is a file, copy it directly
				*
			*/

			else {
				QString source = src;
				QString target = QDir( job->target ).filePath( baseName( src ) );

				if ( isFile( src ) )
					copyFile( job, source, target );

				else if ( isDir( src ) )
					copyDir( job, source, target );

				else
					job->errorNodes << src;
			}
		}
	}

	// We need to move the files
	else {
		NBDeleteManager *deleter = new NBDeleteManager( NULL, false );

		foreach( QString src, job->sources ) {
			/*
				*
				* If the files from the current folder are being pasted into the same folder
				* then we have a problem
				*
			*/
			if ( dirName( src ) == job->target ) {
				job->errorNodes << src;
			}

			/*
				*
				* The files and folder to be pasted do not originate from this directory.
				* But check for the existence of similarly named files/folders.
				* If similarly named files/folders exist, then we follow the above procedure.
				*
			*/
			else if ( exists( QDir( job->target ).filePath( baseName( src ) ) ) ) {
				QString source = src;
				QString target = QDir( job->target ).filePath( "Copy of " + baseName( src ) );

				if ( isFile( src ) ) {
					if ( copyFile( job, source, target ) )
						QFile::remove( source );
				}

				else if ( isDir( src ) ) {
					if ( copyDir( job, source, target ) )
						deleter->deleteFromDisk( QStringList() << source );
				}

				else {
					job->errorNodes << source;
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
				QString target = QDir( job->target ).filePath( baseName( src ) );

				if ( isFile( src ) ) {
					if ( copyFile( job, source, target ) )
						QFile::remove( source );
				}

				else if ( isDir( src ) ) {
					if ( copyDir( job, source, target ) )
						deleter->deleteFromDisk( QStringList() << source );
				}

				else {
					job->errorNodes << source;
				}
			}
		}
	}

	job->running = false;
	job->completed = true;
};

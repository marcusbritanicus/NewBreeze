/*
	*
	* NBArchive.cpp - Archive class for NewBreeze
	*
*/

#include <NBArchive.hpp>

inline QString getTempArchiveName( QString name ) {

	return QDir::temp().filePath( MD5( name ) );
};

QString NBArchive::archiveName = QString();
NBArchive::Mode NBArchive::archiveMode = NBArchive::READ;
NBArchive::Type NBArchive::archiveType = NBArchive::TAR;

NBArchive::NBArchive( QString archive, NBArchive::Mode mode, NBArchive::Type type ) {

	archiveName = QString( archive );
	archiveMode = mode;
	archiveType = type;
};

void NBArchive::updateInputFiles( QStringList inFiles ) {

	inputList.append( inFiles );
	inputList.sort();
	inputList.removeDuplicates();
}

void NBArchive::setWorkingDir( QString wDir ) {

	src = QString( wDir );
};

void NBArchive::setDestination( QString path ) {

	/*
		*
		* @p path will be a absolute.
		* So QDir we construct will be home path
		*
	*/

	dest = QString( path );
	if ( not QFileInfo( dest ).exists() )
		QDir::home().mkpath( path );
};

void NBArchive::create() {

	switch( archiveType ) {
		case NBArchive::TAR: {
			createTar();
			break;
		}

		case NBArchive::ZIP: {
			createZip();
			break;
		}

		case NBArchive::BZ2: {
			createBZ2();
			break;
		}

		case NBArchive::GZ: {
			createGZ();
			break;
		}

		case NBArchive::XZ: {
			createXZ();
			break;
		}
	}
};

void NBArchive::extract() {

	switch( archiveType ) {
		case NBArchive::TAR: {
			extractTar();
			break;
		}

		case NBArchive::ZIP: {
			extractZip();
			break;
		}

		case NBArchive::BZ2: {
			extractBZ2();
			break;
		}

		case NBArchive::GZ: {
			extractGZ();
			break;
		}

		case NBArchive::XZ: {
			extractXZ();
			break;
		}
	}
};

void NBArchive::createTar() {

	TarFile tf( archiveName, TarFile::Write, TarFile::AUTOFilter );
	Q_FOREACH( QString infile, inputList )
		if ( isDir( infile ) )
			tf.addDir( infile );

		else
			tf.addFile( infile );

	tf.writeArchive();
	tf.close();
};

void NBArchive::createZip() {

	ZipFile zf( archiveName, ZipFile::Write );
	Q_FOREACH( QString infile, inputList )
		if ( isDir( infile ) )
			zf.addDir( infile );

		else
			zf.addFile( infile );

	zf.writeArchive();
	zf.close();
};

void NBArchive::createBZ2() {

	Q_FOREACH( QString input, inputList ) {
		NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::WRITE, input );
		archive->create();
	}
};

void NBArchive::createGZ() {

	Q_FOREACH( QString input, inputList ) {
		NBGZip *archive = new NBGZip( archiveName, NBGZip::WRITE, input );
		archive->create();
	}
};

void NBArchive::createXZ() {

	Q_FOREACH( QString input, inputList ) {
		NBLzma *archive = new NBLzma( archiveName, NBLzma::WRITE, input  );
		archive->create();
	}
};

void NBArchive::extractTar() {

	TarFile tf( archiveName, TarFile::Read, TarFile::AUTOFilter );
	tf.readArchive();
	tf.extract();
	tf.close();
};

void NBArchive::extractZip() {

	ZipFile zf( archiveName, ZipFile::Read );
	zf.readArchive();
	zf.extract();
	zf.close();
};

void NBArchive::extractBZ2() {

	NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::READ, dest );
	archive->extract();
};

void NBArchive::extractGZ() {

	NBGZip *archive = new NBGZip( archiveName, NBGZip::READ, dest );
	archive->extract();
};

void NBArchive::extractXZ() {

	NBLzma *archive = new NBLzma( archiveName, NBLzma::READ, dest );
	archive->extract();
};

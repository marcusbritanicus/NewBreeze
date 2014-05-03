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
NBArchive::Type NBArchive::archiveType = NBArchive::TXZ;

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

		case NBArchive::TZIP: {
			tempArchiveName = getTempArchiveName( archiveName );
			createTZip();
			break;
		}

		case NBArchive::TBZ2: {
			tempArchiveName = getTempArchiveName( archiveName );
			createTBZ2();
			break;
		}

		case NBArchive::TGZ: {
			tempArchiveName = getTempArchiveName( archiveName );
			createTGZ();
			break;
		}

		case NBArchive::TXZ: {
			tempArchiveName = getTempArchiveName( archiveName );
			createTXZ();
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

		case NBArchive::TZIP: {
			tempArchiveName = getTempArchiveName( archiveName );
			extractTZip();
			break;
		}

		case NBArchive::TBZ2: {
			tempArchiveName = getTempArchiveName( archiveName );
			extractTBZ2();
			break;
		}

		case NBArchive::TGZ: {
			tempArchiveName = getTempArchiveName( archiveName );
			extractTGZ();
			break;
		}

		case NBArchive::TXZ: {
			tempArchiveName = getTempArchiveName( archiveName );
			extractTXZ();
			break;
		}
	}
};

void NBArchive::createTar() {

	NBTar *archive = new NBTar( archiveName, NBTar::WRITE );
	archive->create( inputList, src );
};

void NBArchive::createZip() {

	NBZip *archive = new NBZip( archiveName, NBZip::WRITE );
	archive->create( inputList, src );
};

void NBArchive::createBZ2() {

	NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::WRITE, inputList.at( 0 ) );
	archive->create();
};

void NBArchive::createGZ() {

	NBGZip *archive = new NBGZip( archiveName, NBGZip::WRITE, inputList.at( 0 )  );
	archive->create();
};

void NBArchive::createXZ() {

	NBLzma *archive = new NBLzma( archiveName, NBLzma::WRITE, inputList.at( 0 )  );
	archive->create();
};

void NBArchive::createTZip() {

	// Create a temporary tar archive
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::WRITE );
	archiveT->create( inputList, src );

	// Now compress it
	NBZip *archive = new NBZip( "/tmp/trial.tzip", NBZip::WRITE );
	archive->create( QStringList() << tempArchiveName, src );

	QFile::remove( tempArchiveName );
};

void NBArchive::createTBZ2() {

	// Create a temporary tar archive
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::WRITE );
	archiveT->create( inputList, src );

	// Now compress it
	NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::WRITE, tempArchiveName );
	archive->create();

	QFile::remove( tempArchiveName );
};

void NBArchive::createTGZ() {

	// Create a temporary tar archive
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::WRITE );
	archiveT->create( inputList, src );

	// Now compress it
	NBGZip *archive = new NBGZip( archiveName, NBGZip::WRITE, tempArchiveName );
	archive->create();

	QFile::remove( tempArchiveName );
};

void NBArchive::createTXZ() {

	// Create a temporary tar archive
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::WRITE );
	archiveT->create( inputList, src );

	// Now compress it
	NBLzma *archive = new NBLzma( archiveName, NBLzma::WRITE, tempArchiveName );
	archive->create();

	QFile::remove( tempArchiveName );
};


void NBArchive::extractTar() {

	NBTar *archive = new NBTar( archiveName, NBTar::READ );
	archive->extract( dest );
};

void NBArchive::extractZip() {

	NBZip *archive = new NBZip( archiveName, NBZip::READ );
	archive->extract( dest );
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

void NBArchive::extractTZip() {

	// Extract Zip to temp Location
	NBZip *archive = new NBZip( archiveName, NBZip::READ );
	archive->extract( tempArchiveName );

	// Now extract it completely
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::READ );
	archiveT->extract( dest );

	QFile::remove( tempArchiveName );
};

void NBArchive::extractTBZ2() {

	// Extract BZip2 to temp Location
	NBBZip2 *archive = new NBBZip2( archiveName, NBBZip2::READ, tempArchiveName );
	archive->extract();

	// Now extract it completely
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::READ );
	archiveT->extract( dest );

	QFile::remove( tempArchiveName );
};

void NBArchive::extractTGZ() {

	// Extract Zip to temp Location
	NBGZip *archive = new NBGZip( archiveName, NBGZip::READ, tempArchiveName );
	archive->extract();

	// Now extract it completely
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::READ );
	archiveT->extract( dest );

	QFile::remove( tempArchiveName );
};

void NBArchive::extractTXZ() {

	// Extract Zip to temp Location
	NBLzma *archive = new NBLzma( archiveName, NBLzma::READ, tempArchiveName );
	archive->extract();

	// Now extract it completely
	NBTar *archiveT = new NBTar( tempArchiveName, NBTar::READ );
	archiveT->extract( dest );

	QFile::remove( tempArchiveName );
};

/*
	*
	* NBTarUtils.hpp - Some functions for NBTar
	*
*/

#include <NBTarUtils.hpp>

bool isDir( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) == 0 )

		if ( S_ISDIR( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

QString dirName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( qPrintable( path ) );
	QString dirPth = QString( dirname( dupPath ) ) + "/";
	free( dupPath );

	return dirPth;
};

QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( qPrintable( path ) );
	QString basePth = QString( basename( dupPath ) );
	free( dupPath );

	return basePth;
};

bool exists( QString path ) {

	return not access( qPrintable( path ), F_OK );
}

bool isReadable( QString path ) {

	if ( isDir( path ) )
		return not access( path.toLocal8Bit().data(), R_OK | X_OK );

	else
		return not access( path.toLocal8Bit().data(), R_OK );
};

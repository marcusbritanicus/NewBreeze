/*
	*
	* NBTools.cpp - Some tools for NewBreeze
	*
*/

#include <NBTools.hpp>

QString dirName( QString path ) {

	while( path.contains( "//" ) )
		path = path.replace( "//", "/" );

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( path.toLocal8Bit().data() );
	QString dirPth = QString( dirname( dupPath ) ) + "/";
	free( dupPath );

	return dirPth;
};

QString baseName( QString path ) {

	while( path.contains( "//" ) )
		path = path.replace( "//", "/" );

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( path.toLocal8Bit().data() );
	QString basePth = QString( basename( dupPath ) );
	free( dupPath );

	return basePth;
};

bool isFile( QString path ) {

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) == 0 )

		if ( S_ISREG( statbuf.st_mode ) or S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

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

bool isLink( QString path ) {

	struct stat statbuf;
	if ( lstat( path.toLocal8Bit().data(), &statbuf ) == 0 )
		if ( S_ISLNK( statbuf.st_mode ) )
			return true;

		else
			return false;

	else
		return false;
};

bool exists( QString path ) {

	return not access( path.toLocal8Bit().data(), F_OK );
}

int mkpath( QString path, mode_t mode ) {

	/* Root always exists */
	if ( path == "/" )
		return 0;

	/* If the directory exists, thats okay for us */
	if ( exists( path ) )
		return 0;

	mkpath( dirName( path ), mode );

	return mkdir( path.toLocal8Bit().data(), mode );
};

bool isReadable( QString path ) {

	if ( isDir( path ) )
		return not access( path.toLocal8Bit().data(), R_OK | X_OK );

	else
		return not access( path.toLocal8Bit().data(), R_OK );
};

bool isWritable( QString path ) {

	return not access( path.toLocal8Bit().data(), W_OK );
};

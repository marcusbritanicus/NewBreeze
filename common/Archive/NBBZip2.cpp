/*
	*
	* Copyright 2016 Britanicus <marcusbritanicus@gmail.com>
	*

	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
	* (at your option) any later version.
	*

	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*

	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
	* MA 02110-1301, USA.
	*
*/

// Local Headers
#include "NBBZip2.hpp"

const int MAX_READ_SIZE = 40960;

QString NBBZip2::bz2FileName = QString();
QString NBBZip2::fileName = QString();

NBBZip2::NBBZip2( QString archive, QString file ) {

	int error = 0;

	bz2FileName = QString( archive );
	if ( not file.isEmpty() ) {
		if ( QFileInfo( file ).isDir() ) {
			fileName = QDir( file ).filePath( QString( archive ) );
			fileName.chop( 4 );
		}

		else if ( QFileInfo( file ).exists() ) {
			QFile::rename( file, file + ".old" );
			fileName = QString( file );
		}

		else {
			fileName = QString( file );
		}
	}

	else {
		fileName = QString( archive );
		fileName.chop( 4 );
	}

	bzFile = fopen( qPrintable( bz2FileName ), "r" );
	bz2 = BZ2_bzReadOpen( &error, bzFile, 0, 0, NULL, 0 );
};

bool NBBZip2::extract() {

	int error;

	// Reading from the bz2 file opened
	std::ofstream ofile( qPrintable( fileName ), std::ofstream::binary );

	while ( true ) {
		char buffer[ MAX_READ_SIZE ] = { "\x00" };
		int charsRead = BZ2_bzRead( &error, bz2, buffer, MAX_READ_SIZE );
		ofile.write( buffer, charsRead );

		if ( error == BZ_OK )
			continue;

		else
			break;
	}

	if ( error != BZ_STREAM_END )
		return false;

	// Close the file
	BZ2_bzReadClose( &error, bz2 );

	if ( error != BZ_OK )
		return false;

	fclose( bzFile );

	return true;
};

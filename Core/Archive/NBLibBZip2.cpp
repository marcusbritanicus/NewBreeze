/*
	*
	* Copyright 2011 Britanicus <marcusbritanicus@gmail.com>
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
#include <NBLibBZip2.hpp>
#include <NBTools.hpp>

const int MAX_READ_SIZE = 40960;

QString NBBZip2::bz2FileName = QString();
QString NBBZip2::fileName = QString();

NBBZip2::NBBZip2( QString archive, NBBZip2::Mode openmode, QString file ) {

	NBBZip2::Mode mode = openmode;
	int error = 0;

	switch( mode ) {
		case NBBZip2::READ : {
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

			bzFile = fopen( bz2FileName.toLocal8Bit().data(), "r" );
			bz2 = BZ2_bzReadOpen( &error, bzFile, 0, 0, NULL, 0 );
			break;
		}

		case NBBZip2::WRITE : {
			bz2FileName = QString( archive );
			fileName = QString( file );

			bzFile = fopen( bz2FileName.toLocal8Bit().data(), "w" );
			bz2 = BZ2_bzWriteOpen( &error, bzFile, 9, 0, 30 );
			break;
		}
	}
};

void NBBZip2::create() {

	int error;
	// Write to the bz2 file created above

	std::ifstream ifile( fileName.toLocal8Bit().data(), std::ifstream::binary );
	off_t fileSize = QFileInfo( fileName ).size();

	if ( fileSize < MAX_READ_SIZE ) {
		char buffer[ MAX_READ_SIZE ] = { "\x00" };
		ifile.read( buffer, fileSize );
		BZ2_bzWrite( &error, bz2, buffer, fileSize );
	}

	else {
		off_t nextChunkSize = MAX_READ_SIZE, remaining = fileSize - MAX_READ_SIZE;
		while ( nextChunkSize > 0 ) {
			char buffer[ MAX_READ_SIZE ] = { "\x00" };
			ifile.read( buffer, nextChunkSize );
			BZ2_bzWrite( &error, bz2, buffer, nextChunkSize );
			nextChunkSize = ( remaining > MAX_READ_SIZE ) ? MAX_READ_SIZE : remaining;
			remaining -= nextChunkSize;
		}
	}

	if ( error != BZ_OK )
		return;

	fflush( bzFile );

	// Close the file
	unsigned int inBytes, outBytes;
	BZ2_bzWriteClose( &error, bz2, 0, &inBytes, &outBytes );

	if ( error != BZ_OK )
		return;

	fclose( bzFile );

	return;
}

void NBBZip2::extract() {

	int error;

	// Reading from the bz2 file opened
	std::ofstream ofile( fileName.toLocal8Bit().data(), std::ofstream::binary );

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
		return;

	// Close the file
	BZ2_bzReadClose( &error, bz2 );

	if ( error != BZ_OK )
		return;

	fclose( bzFile );

	return;
};

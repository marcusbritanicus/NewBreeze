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
#include "NBLzma.hpp"

const int MAX_READ_SIZE = 40960;

QString NBLzma::lzmaFileName = QString();
QString NBLzma::fileName = QString();

NBLzma::NBLzma( QString archive, QString file ) {

	lzmaFileName = QString( archive );
	if ( not file.isEmpty() ) {
		if ( QFileInfo( file ).isDir() ) {
			fileName = QDir( file ).filePath( QString( archive ) );
			fileName.chop( 3 );
		}

		else if ( QFileInfo( file ).exists() ) {
			QFile::rename( file, file + ".old" );
			fileName = QString( file );
		}

		else {
			fileName = QString( file );
		}
	}

	fdin = fopen( qPrintable( lzmaFileName ), "rb" );
	fdout = fopen( qPrintable( fileName ), "wb" );
};

bool NBLzma::extract() {

	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret;

	// Initialize the decoder
	ret = lzma_alone_decoder( &strm, UINT64_MAX );

	if ( ret != LZMA_OK )
		return false;

	uint8_t in_buf[ MAX_READ_SIZE ];
	uint8_t out_buf[ MAX_READ_SIZE ];

	strm.avail_in = 0;
	strm.next_out = out_buf;
	strm.avail_out = MAX_READ_SIZE;

	lzma_action action = LZMA_RUN;

	while ( true ) {
		if ( strm.avail_in == 0 ) {
			strm.next_in = in_buf;
			strm.avail_in = fread( in_buf, 1, MAX_READ_SIZE, fdin );

			if ( ferror( fdin ) )
				return false;
		}

		if ( feof( fdin ) )
			action = LZMA_FINISH;

		ret = lzma_code( &strm, action );

		if ( strm.avail_out == 0 || ret != LZMA_OK ) {
			const size_t write_size = MAX_READ_SIZE - strm.avail_out;

			if ( fwrite( out_buf, 1, write_size, fdout ) != write_size )
				return false;

			strm.next_out = out_buf;
			strm.avail_out = MAX_READ_SIZE;
		}

		if ( ret != LZMA_OK ) {
			if ( ret == LZMA_STREAM_END ) {
				// lzma_stream_decoder() already guarantees that there's no trailing garbage.
				assert( strm.avail_in == 0 );
				assert( action == LZMA_FINISH );
				assert( feof( fdin ) );
				return false;
			}
		}
	}

	return false;
};

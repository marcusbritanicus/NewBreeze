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
#include "NBLibLzma2.hpp"

const int MAX_READ_SIZE = 40960;

QString NBXz::xzFileName = QString();
QString NBXz::fileName = QString();

NBXz::NBXz( QString archive, NBXz::Mode openmode, QString file ) {

	NBXz::Mode mode = openmode;
	lzma_ret ret_xz;
	switch( mode ) {
		case NBXz::READ : {
			xzFileName = QString( archive );
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

			fdin = fopen( qPrintable( xzFileName ), "rb" );
			fdout = fopen( qPrintable( fileName ), "wb" );
			break;
		}

		case NBXz::WRITE : {
			xzFileName = QString( archive );
			fileName = QString( file );

			fdin = fopen( qPrintable( fileName ), "rb" );
			fdout = fopen( qPrintable( xzFileName ), "wb" );
			break;
		}
	}

	Q_UNUSED( ret_xz );
};

void NBXz::create() {

	#define INTEGRITY_CHECK LZMA_CHECK_CRC64
	#define IN_BUF_MAX      40960
	#define OUT_BUF_MAX     40960
	#define RET_OK                  0
	#define RET_ERROR_INIT          1
	#define RET_ERROR_INPUT         2
	#define RET_ERROR_OUTPUT        3
	#define RET_ERROR_COMPRESSION   4

	long long int count=0;

	uint8_t in_buf [IN_BUF_MAX];
	uint8_t out_buf [OUT_BUF_MAX];

	int in_finished = 0;
	int out_finished = 0;

	lzma_action action;
	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret_xz;

	uint32_t preset = 9 | 1;
	lzma_check check = LZMA_CHECK_CRC64;
	ret_xz = lzma_easy_encoder( &strm, preset, check );

	while ( ( !in_finished ) and ( !out_finished ) ) {
		size_t in_len;  /* length of useful data in in_buf */
		size_t out_len; /* length of useful data in out_buf */

		in_len = fread( in_buf, 1, IN_BUF_MAX, fdin );

		if ( feof( fdin ) )
			in_finished = 1;

		if ( ferror ( fdin ) )
			in_finished = 1;

		strm.next_in = in_buf;
		strm.avail_in = in_len;

		action = in_finished ? LZMA_FINISH : LZMA_RUN;

		do {
			strm.next_out = out_buf;
			strm.avail_out = OUT_BUF_MAX;

			ret_xz = lzma_code( &strm, action );

			if ( ( ret_xz != LZMA_OK ) && ( ret_xz != LZMA_STREAM_END ) ) {
				out_finished = 1;
				return;
			}

			else {
				out_len = OUT_BUF_MAX - strm.avail_out;
				count+=out_len;
				fwrite ( out_buf, 1, out_len, fdout );
				if ( ferror ( fdout ) ){
					out_finished = 1;
					return;
				}
			}
		} while ( strm.avail_out == 0 );
	}

	lzma_end ( &strm );
	fclose( fdin );
	fclose( fdout );
}

void NBXz::extract() {

	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret;

	// Initialize the decoder
	ret = lzma_stream_decoder( &strm, UINT64_MAX, LZMA_CONCATENATED );

	if ( ret != LZMA_OK )
		return;

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
				return;
		}

		if ( feof( fdin ) )
			action = LZMA_FINISH;

		ret = lzma_code( &strm, action );

		if ( strm.avail_out == 0 || ret != LZMA_OK ) {
			const size_t write_size = MAX_READ_SIZE - strm.avail_out;

			if ( fwrite( out_buf, 1, write_size, fdout ) != write_size )
				return;

			strm.next_out = out_buf;
			strm.avail_out = MAX_READ_SIZE;
		}

		if ( ret != LZMA_OK ) {
			if ( ret == LZMA_STREAM_END ) {
				// lzma_stream_decoder() already guarantees that there's no trailing garbage.
				assert( strm.avail_in == 0 );
				assert( action == LZMA_FINISH );
				assert( feof( fdin ) );
				return;
			}
		}
	}

	return;
};

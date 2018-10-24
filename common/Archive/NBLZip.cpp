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
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#include "NBLZip.hpp"

#ifndef O_BINARY
	#define O_BINARY 0
#endif

#ifndef max
	#define max( x, y ) ( ( x ) >= ( y ) ? ( x ) : ( y ) )
#endif

#ifndef min
	#define min( x , y ) ( ( x ) <= ( y ) ? ( x ) : ( y ) )
#endif

const int MAX_READ_SIZE = 40960;
const int BUFFER_SIZE = 6553;

NBLZip::NBLZip( QString archive, QString file ) {

	lzFileName = QString( archive );
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

	else {

		/* Remove the last 3 */
		fileName = lzFileName;
		fileName.chop( 3 );
	}
};

bool NBLZip::extract() {

	/* Prepare input */
	int infd = open( lzFileName.toLocal8Bit().data(), O_RDONLY | O_BINARY );

	/* Prepare output */
	int outfd = open( fileName.toLocal8Bit().data(), O_CREAT | O_WRONLY | O_BINARY, S_IRUSR | S_IWUSR );

	struct LZ_Decoder * const decoder = LZ_decompress_open();

	uint8_t buffer[ BUFFER_SIZE ];
	bool first_member;

	for ( first_member = true; ; ) {
		const int max_in_size = min( LZ_decompress_write_size( decoder ), BUFFER_SIZE );
		int in_size = 0, out_size = 0;

		if( max_in_size > 0 ) {
			in_size = readblock( infd, buffer, max_in_size );
			if( in_size != max_in_size && errno ) {
				qDebug() << "Read error. Code" << errno;
				return 1;
			}

			if( in_size > 0 && in_size != LZ_decompress_write( decoder, buffer, in_size ) )
				qDebug() << "Library error (LZ_decompress_write).";

			if( in_size < max_in_size )
				LZ_decompress_finish( decoder );
		}

		while( true ) {
			const int rd = LZ_decompress_read( decoder, buffer, BUFFER_SIZE );
			if( rd > 0 ) {
				out_size += rd;
				if( outfd >= 0 ) {
					const int wr = writeblock( outfd, buffer, rd );
					if( wr != rd ) {
						qDebug() << "Write error. Code" << errno;
						return 1;
					}
				}
			}

			else if( rd < 0 ) {
				out_size = rd; break;
			}

			if( LZ_decompress_member_finished( decoder ) == 1 ) {
				first_member = false;
			}
			if( rd <= 0 )
				break;
		}

		if( out_size < 0 || ( first_member && out_size == 0 ) ) {
			const unsigned long long member_pos = LZ_decompress_member_position( decoder );
			const enum LZ_Errno lz_errno = LZ_decompress_errno( decoder );

			if( lz_errno == LZ_library_error )
				qDebug() << "Library error (LZ_decompress_read).";

			if( member_pos <= 6 ) {
				if( lz_errno == LZ_unexpected_eof ) {
					if ( first_member )
						qDebug() << "ERROR: File ends unexpectedly at member header.";

					else
						qDebug() << "ERROR: Truncated header in multimember file.";

					return false;
				}

				else if( lz_errno == LZ_data_error ) {

					if( member_pos == 4 ) {
						qDebug() << "Version" << LZ_decompress_member_version( decoder ) << "member format not supported.";
					}

					else if( member_pos == 5 )
						qDebug() << "Invalid dictionary size in member header.";

					else if( first_member )	/* for lzlib older than 1.10 */
						qDebug() << "Bad version or dictionary size in member header.";

					else
						break;				/* trailing data */

					return false;
				}
			}

			if ( lz_errno == LZ_header_error ) {
				if ( first_member )
					qDebug() << "ERROR: Bad magic number (file not in lzip format).";

				else
					break;				/* trailing data */

				return false;
			}

			if( lz_errno == LZ_mem_error ) {
				qDebug() << "Not enough memory.";
				return 1;
			}

			return false;
		}

		if ( LZ_decompress_finished( decoder ) == 1 )
			break;

		if ( in_size == 0 && out_size == 0 )
			qDebug() << "ERROR: Library error (stalled).";
	}

	LZ_decompress_close( decoder );

	return true;
};

int NBLZip::readblock( const int fd, uint8_t * const buf, const int size ) {
	int sz = 0;
	errno = 0;
	while( sz < size ) {
		const int n = read( fd, buf + sz, size - sz );
		if( n > 0 ) sz += n;
		else if( n == 0 ) break;				/* EOF */
		else if( errno != EINTR ) break;
		errno = 0;
	}

	return sz;
}

int NBLZip::writeblock( const int fd, const uint8_t * const buf, const int size ) {

	int sz = 0;
	errno = 0;
	while( sz < size ) {
		const int n = write( fd, buf + sz, size - sz );
		if( n > 0 )
			sz += n;
		else if ( n < 0 && errno != EINTR )
			break;

		errno = 0;
	}
	return sz;
};

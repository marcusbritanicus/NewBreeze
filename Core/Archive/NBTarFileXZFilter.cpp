/*
	*
	* Copyright 2011 Britanicus <marcusbritanicus@gmail.com>
	*
*/

// C++ Standard Library
#include <iostream>
#include <fstream>

// C Standard Library
#include <cstring>
#include <cstdlib>

// SystemWide Headers
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>

// Local Headers
#include <NBTarFile.hpp>

/*
	*
	* Until such a time as xz/lzma provides a zlib style interface, I will be writing the tar to /tmp/.
	* The thus written tar, will then be compressed using the standard lzma routine to produce a proper output.
	*
*/

bool TarFile::readArchiveXZ( QString target ) {

	QString tmpTar = QDir::temp().filePath( MD5( mFileName ) );
	FILE *fdin, *fdout;

	fdin = fopen( mFileName.toLocal8Bit().data(), "rb" );
	fdout = fopen( tmpTar.toLocal8Bit().data(), "wb" );

	lzma_stream strm = LZMA_STREAM_INIT;
	lzma_ret ret;

	// Initialize the decoder
	ret = lzma_stream_decoder( &strm, UINT64_MAX, LZMA_CONCATENATED );

	if ( ret != LZMA_OK )
		return false;

	uint8_t in_buf[ BUFSIZ ];
	uint8_t out_buf[ BUFSIZ ];

	strm.avail_in = 0;
	strm.next_out = out_buf;
	strm.avail_out = BUFSIZ;

	lzma_action action = LZMA_RUN;

	while ( true ) {
		if ( strm.avail_in == 0 ) {
			strm.next_in = in_buf;
			strm.avail_in = fread( in_buf, 1, BUFSIZ, fdin );

			if ( ferror( fdin ) )
				return false;
		}

		if ( feof( fdin ) )
			action = LZMA_FINISH;

		ret = lzma_code( &strm, action );

		if ( strm.avail_out == 0 || ret != LZMA_OK ) {
			const size_t write_size = BUFSIZ - strm.avail_out;

			if ( fwrite( out_buf, 1, write_size, fdout ) != write_size )
				return false;

			strm.next_out = out_buf;
			strm.avail_out = BUFSIZ;
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

	fclose( fdin );
	fclose( fdout );

	TarFile tf( tmpTar, TarFile::Read, TarFile::AUTOFilter );
	tf.readArchive();
	tf.extract( target );
	tf.close();

	return true;
};

bool TarFile::writeArchiveXZ() {

	/* Open the .tar file for IO */
	tarFile.open( QDir::temp().filePath( MD5( mFileName ) ).toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary );

	if ( not tarFile.is_open() ) {
		qDebug() << "Error opening archive file" << mFileName;
		return false;
	}

	for( QString fn: files ) {
		/* Get the header */
		TarHeader *header = TarHeader::createHeader( fn );
		tarFile.write( header->headerString().data(), 512 );

		/* We will adding file contents to the archive only if it's a regular file */
		if ( header->value( TarHeader::Type ).toInt() == '0' ) {
			/* block size should be an integral multiple of 512 */
			/* if it is not add a null buffer */
			int bufLen = 512 - header->value( TarHeader::Size ).toLongLong() % 512;

			/* Read and write the input file */
			std::ifstream infile;
			infile.open( fn.toLocal8Bit().data() );
			while ( not infile.eof() ) {
				char *buffer = new char[BUFSIZ];
				infile.read( buffer, BUFSIZ );
				tarFile.write( buffer, infile.gcount() );
			}

			/* Write the buffer to fill 512 byte block */
			if ( bufLen < 512 )
				tarFile.write( QByteArray( bufLen, '\0' ).data(), bufLen );
			tarFile.flush();
		}
	}

	tarFile.write( QByteArray( 1024, '\0' ).data(), 1024 );
	tarFile.flush();

	files.clear();

	/* Input file: out temporary tarfile */
	FILE *fdin = fopen( QDir::temp().filePath( MD5( mFileName ) ).toLocal8Bit().data(), "rb" );

	/* Output file: out .txz file */
	FILE *fdout = fopen( mFileName.toLocal8Bit().data(), "wb" );

	/* Alloc and init lzma_stream struct */
	lzma_stream strm = LZMA_STREAM_INIT;

	/* Input buffer */
	uint8_t in_buf[ BUFSIZ ];

	/* Output buffer */
	uint8_t out_buf[ BUFSIZ ];

	/* length of useful data in in_buf */
	size_t in_len;

	/* length of useful data in out_buf */
	size_t out_len;

	/* Action lzma_code function should perform */
	lzma_action action;

	/* Return value of lzma_* functions */
	lzma_ret ret_xz;

	/* Init the lzma_stream object */
	ret_xz = lzma_easy_encoder( &strm, 9 | 1, LZMA_CHECK_CRC64 );

	/* Check if the stream was init properly */
	if ( ret_xz != LZMA_OK ) {

		qDebug() << "Error initializing the lzma stream:" << ret_xz;
		return false;
	}

	/* Read and compress the data */
	while ( !feof( fdin ) ) {
		/* Read the data from the input file */
		in_len = fread( in_buf, 1, BUFSIZ, fdin );

		/* Set the stream input pointer to the buffer */
		strm.next_in = in_buf;

		/* Set the stream input length */
		strm.avail_in = in_len;

		/* Set the action to be performed */
		action = feof( fdin ) ? LZMA_FINISH : LZMA_RUN;

		/* Do the data crunching and write the output */
		do {
			/* Output buffer is empty */
			strm.next_out = out_buf;

			/* lzma stream has full BUFSIZ space available */
			strm.avail_out = BUFSIZ;

			/* Compress the input data */
			ret_xz = lzma_code( &strm, action );

			/* If the output is not proper, abort */
			if ( ( ret_xz != LZMA_OK ) &&( ret_xz != LZMA_STREAM_END ) ) {

				qDebug() << "Error compressing data:" << ret_xz;
				return false;
			}

			/* Write whatever was in the output buffer */
			else {
				/* Get the size of outbuf */
				out_len = BUFSIZ - strm.avail_out;

				/* Write the output */
				fwrite( out_buf, 1, out_len, fdout );

				/* Check if there was any error writing the output */
				if ( ferror( fdout ) ) {

					qDebug() << "Error writing the output";
					return false;
				}
			}
		} while ( strm.avail_out == 0 );
	}

	/* Close the lzma stream */
	lzma_end( &strm );

	/* Close the input */
	fclose( fdin );

	/* Close the output */
	fclose( fdout );

	/* Remove the temporary tar file */
	remove( QDir::temp().filePath( MD5( mFileName ) ).toLocal8Bit().data() );

	return true;
};

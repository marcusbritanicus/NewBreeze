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

bool TarFile::readArchiveBZ2( QString target ) {

	QString tmpTar = QDir::temp().filePath( MD5( mFileName ) );
	int error;

	std::ofstream ofile( tmpTar.toLocal8Bit().data(), std::ofstream::binary );

	FILE *bzFile = fopen( qPrintable( mFileName ), "w" );
	BZFILE *bz2 = BZ2_bzWriteOpen( &error, bzFile, 9, 0, 30 );

	while ( true ) {
		char buffer[ BUFSIZ ] = { "\x00" };

		// Reading from the bz2 file opened
		int charsRead = BZ2_bzRead( &error, bz2, buffer, BUFSIZ );

		/* Writing to the output file */
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
	ofile.close();

	TarFile tf( tmpTar, TarFile::Read, TarFile::AUTOFilter );
	tf.readArchive();
	tf.extract( target );
	tf.close();

	return true;
};

bool TarFile::writeArchiveBZ2() {

	/* BZ2 Error */
	int error;

	/* File object for BZ2 Compressor */
	FILE *bzFile = fopen( mFileName.toLocal8Bit().data(), "w" );

	/* BZ2 Handle */
	BZFILE *bz2 = BZ2_bzWriteOpen( &error, bzFile, 9, 0, 30 );
	qDebug() << "Init bz2 stream";

	if ( error != BZ_OK ) {
		qDebug() << "Error initializing BZ2 Stream";
		return false;
	}

	for( QString fn: files ) {
		/* Get the header */
		TarHeader *header = TarHeader::createHeader( fn );

		// Write the TarHeader
		qDebug() << "Writing" << fn;
		BZ2_bzWrite( &error, bz2, header->headerString().data(), 512 );

		/* We will adding file contents to the archive only if it's a regular file */
		if ( header->value( TarHeader::Type ).toInt() == '0' ) {
			/* block size should be an integral multiple of 512 */
			/* if it is not add a null buffer */
			int bufLen = 512 - header->value( TarHeader::Size ).toLongLong() % 512;

			/* Read and write the input file */
			std::ifstream infile;
			infile.open( fn.toLocal8Bit().data() );
			while( not infile.eof() ) {
				char *buffer = new char[BUFSIZ];
				infile.read( buffer, BUFSIZ );
				BZ2_bzWrite( &error, bz2, buffer, infile.gcount() );
			}
			infile.close();

			/* Write the buffer to fill 512 byte block */
			if ( bufLen < 512 )
				BZ2_bzWrite( &error, bz2, QByteArray( bufLen, '\0' ).data(), bufLen );

			/* Flush the file stream */
			fflush( bzFile );
		}
	}

	/* Write the end blocks */
	BZ2_bzWrite( &error, bz2, QByteArray( 1024, '\0' ).data(), 1024 );

	/* Flush the file stream */
	fflush( bzFile );

	/* Close the BZ2 Stream */
	unsigned int inBytes, outBytes;
	BZ2_bzWriteClose( &error, bz2, 0, &inBytes, &outBytes );

	/* Close the file and clear the input queue */
	fclose( bzFile );
	files.clear();

	if ( error == BZ_OK ) {

		return true;
	}

	else {

		qDebug() << "BZ2_bzWriteClose() returned: " << error;
		return false;
	}

	return false;
};

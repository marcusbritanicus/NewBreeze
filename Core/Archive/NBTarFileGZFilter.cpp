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

bool TarFile::readArchiveGZ( QString target ) {

	/* Open GZip file fro reading */
	gzFile gzip = gzopen( qPrintable( mFileName ), "rb" );

	/*  */
	if ( gzip == NULL )
		return false;

	while ( true ) {
		/* Allocate buffer for reading decompresed data */
		char *buffer = new char[ BUFSIZ ];

		/* Read the file and decompress the data */
		int charsRead = gzread( gzip, buffer, sizeof( buffer ) );

		/*
			*
			* Process the data
			* The first 512 bytes will be the header. Decode it.
			* Then extract the file
			* After that, dynamically detect the header and extract the files
			*
		*/

		off_t fbytes_read = 0;
		off_t fbytes_total = 0;
		if ( charsRead > 0 ) {
			char *buf = new char[ 512 ];
			strncat( buf, buffer, 512 );

			TarHeader *hdr = TarHeader::createHeader( buf );
			QString fName = hdr->value( TarHeader::Name ).toString();

			switch( hdr->value( TarHeader::Type ).toChar().toAscii() ) {
				/* Regular file */
				case '0' : {
					/* Get the target file size */
					fbytes_total = hdr->value( TarHeader::Size ).toLongLong();

					/* Create the C++ file object */
					std::ofstream f;

					/* Create the underlying path */
					QDir( target ).mkpath( dirName( fName ) );

					/* Open the C++ File Object */
					f.open( fName.toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary );

					/* Buffer to read the file into */
					char *buf;

					/* If the file is smaller than BUFSIZ read it at once */
					if ( fbytes_total < BUFSIZ ) {
						buf = new char[ fbytes_total ];
						tarFile.read( buf, fbytes_total );

						f.write( buf, fbytes_total );
						f.close();
					}

					/* The File is larger than BUFSIZ, read it chunk by chunk */
					else {
						/* Bytes remaining to be read */
						off_t remaining = fbytes_total;

						/* Read and write until there is nothing to be read */
						while( remaining > 0 ) {
							/* If the remaining bytes are more than BUFSIZ, read just BUFSIZ bytes and write them */
							if ( remaining > BUFSIZ ) {
								buf = new char[ BUFSIZ ];
								tarFile.read( buf, BUFSIZ );

								f.write( buf, BUFSIZ );
								remaining -= BUFSIZ;
							}

							/* If the remaining bytes are less than BUFSIZ, read them all and write them */
							else {
								buf = new char[ remaining ];
								tarFile.read( buf, remaining );

								f.write( buf, remaining );
								remaining -= remaining;
							}
						}

						/* Close the file object */
						f.close();
					}

					/* Set the file property: mode */
					chmod( fName.toLocal8Bit().data(), hdr->value( TarHeader::Mode ).toLongLong() );

					/* Set the dir property: mode */
					struct utimbuf times;
					times.actime = hdr->value( TarHeader::Time ).toLongLong();
					times.modtime = hdr->value( TarHeader::Time ).toLongLong();
					utime( fName.toLocal8Bit().data(), &times );

					break;
				}

				/* SymLink */
				case '2' : {
					/* Create the underlying path */
					QDir( target ).mkpath( dirName( fName ) );

					QString path = hdr->value( TarHeader::Target ).toString();
					symlink( path.toLocal8Bit().data(), fName.toLocal8Bit().data() );

					break;
				}

				/* Directory */
				case '5' : {
					/* Create the directory */
					QDir( target ).mkpath( fName );

					/* Set the dir property: mode */
					chmod( fName.toLocal8Bit().data(), hdr->value( TarHeader::Mode ).toLongLong() );

					/* Set the dir property: time */
					struct utimbuf times;
					times.actime = hdr->value( TarHeader::Time ).toLongLong();
					times.modtime = hdr->value( TarHeader::Time ).toLongLong();
					utime( fName.toLocal8Bit().data(), &times );

					break;
				}

				/* For others do nothing: until we learn how to create the respective objects: FIFO, BLOCKDEV, CHARDEV etc */
				default: {

					break;
				}
			}
		}

		else if ( charsRead == 0 )
			break;

		else
			return false;
	}

	return true;
};

bool TarFile::writeArchiveGZ() {

	/* GZ Error */
	int error;

	/* File object for GZ Compressor */
	gzFile gzip = gzopen( mFileName.toLocal8Bit().data(), "wb" );

	if ( gzip == NULL ) {
		qDebug() << "Error initializing GZ Stream";
		return false;
	}

	for( QString fn: files ) {
		/* Get the header */
		TarHeader *header = TarHeader::createHeader( fn );

		// Write the TarHeader
		if ( gzwrite( gzip, header->headerString().data(), 512 ) != 512 ) {
			gzerror( gzip, &error );
			gzclose( gzip );
			return false;
		}

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
				 if ( gzwrite( gzip, buffer, infile.gcount() ) != infile.gcount() ) {
					gzerror( gzip, &error );
					gzclose( gzip );
					return false;
				 }
			}
			infile.close();

			/* Write the buffer to fill 512 byte block */
			if ( bufLen < 512 ) {
				if ( gzwrite( gzip, QByteArray( bufLen, '\0' ).data(), bufLen ) != bufLen ) {
					gzerror( gzip, &error );
					gzclose( gzip );
					return false;
				}
			}
		}
	}

	/* Write the tar end blocks */
	if ( gzwrite( gzip, QByteArray( 1024, '\0' ).data(), 1024 ) != 1024 ) {
		gzerror( gzip, &error );
		gzclose( gzip );
		return false;
	}

	/* Close the file and clear the input queue */
	files.clear();

	/* Close the GZ Stream */
	if ( gzclose( gzip ) != Z_OK ) {
		qDebug() << "gzclose() returned: " << error;
		return false;
	}

	else {

		return true;
	}

	return false;
};

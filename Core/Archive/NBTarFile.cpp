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

	*
	* This is a C++ tar library for Qt4/Qt5. Apart from writing raw tar
	* archives, this library can also use three filters: xz, bz2, gz.
	*
	* Please note that this library writes the compressed data on-the-fly,
	* without writing an intermediate tar file.
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

TarFile::TarFile( QString fileName, TarFile::OpenMode mode, TarFile::Filter filter ) {

	switch( mode ) {
		case TarFile::Read: {
			switch( filter ) {
				case TarFile::NOFilter:
				case TarFile::XZFilter:
				case TarFile::GZFilter:
				case TarFile::BZ2Filter: {
					qDebug() << "Ignoring the given value of TarFile::Filter.";
					qDebug() << "Using TarFile::AUTOFilter to determine the archive type.";
				}

				case TarFile::AUTOFilter: {
					if ( fileName.endsWith( ".tar.xz" ) or fileName.endsWith( ".txz" ) )
						mFilter = TarFile::XZFilter;

					else if ( fileName.endsWith( ".tar.gz" ) or fileName.endsWith( ".tgz" ) )
						mFilter = TarFile::XZFilter;

					else if ( fileName.endsWith( ".tar.bz2" ) or fileName.endsWith( ".tbz2" ) )
						mFilter = TarFile::BZ2Filter;

					else if ( fileName.endsWith( ".tar" ) )
						mFilter = TarFile::NOFilter;

					else {
						qDebug() << "You are using AUTOFilter and no valid extension was detected.";
						qDebug() << "Attempting to determine the archive type based on mimetype.";
						mFilter = TarFile::NOFilter;
					}

					break;
				}
			}

			/* Flag to check if the archive has been read once */
			mRead = false;

			/* Set the mFileName value */
			mFileName = fileName;

			/* Set the mMode value */
			mMode = mode;

			/* Open the file for IO */
			tarFile.open( fileName.toLatin1().data(), std::ios_base::in | std::ios_base::binary );

			break;
		}

		case TarFile::Write: {
			switch( filter ) {
				case TarFile::NOFilter: {

					if ( not fileName.endsWith( ".tar" ) ) {
						qDebug() << "You are using NOFilter. Adding '.tar' suffix to filename.";
						fileName += ".tar";
					}

					mFilter = TarFile::NOFilter;
					break;
				}

				case TarFile::XZFilter: {

					if ( not ( fileName.endsWith( ".tar.xz" ) or fileName.endsWith( ".txz" ) ) ) {
						qDebug() << "You are using XZFilter. Adding '.tar.xz' suffix to filename.";
						fileName += ".tar.xz";
					}

					mFilter = TarFile::XZFilter;
					break;
				}

				case TarFile::GZFilter: {

					if ( not ( fileName.endsWith( ".tar.gz" ) or fileName.endsWith( ".tgz" ) ) ) {
						qDebug() << "You are using GZFilter. Adding '.tar.gz' suffix to filename.";
						fileName += ".tar.gz";
					}

					mFilter = TarFile::GZFilter;
					break;
				}

				case TarFile::BZ2Filter: {

					if ( not ( fileName.endsWith( ".tar.bz2" ) or fileName.endsWith( ".tbz2" ) ) ) {
						qDebug() << "You are using BZ2Filter. Adding '.tar.bz2' suffix to filename.";
						fileName += ".tar.bz2";
					}

					mFilter = TarFile::BZ2Filter;
					break;
				}

				case TarFile::AUTOFilter: {
					if ( fileName.endsWith( ".tar.xz" ) or fileName.endsWith( ".txz" ) )
						mFilter = TarFile::XZFilter;

					else if ( fileName.endsWith( ".tar.gz" ) or fileName.endsWith( ".tgz" ) )
						mFilter = TarFile::XZFilter;

					else if ( fileName.endsWith( ".tar.bz2" ) or fileName.endsWith( ".tbz2" ) )
						mFilter = TarFile::BZ2Filter;

					else if ( fileName.endsWith( ".tar" ) )
						mFilter = TarFile::NOFilter;

					else {
						qDebug() << "You are using AUTOFilter and no valid extension was detected.";
						qDebug() << "Using NOFilter. Adding '.tar' suffix to filename.";
						mFilter = TarFile::NOFilter;
					}

					break;
				}
			}

			/* Set the mFileName value */
			mFileName = fileName;

			/* Set the mMode value */
			mMode = mode;

			/* Open the file for IO */
			// This will be done in the respective filtered writers

			break;
		}
	}
};

bool TarFile::isOpen() {

	return tarFile.is_open();
};

bool TarFile::readArchive() {

	mRead = true;

	/* To get the tarFile size */
	struct stat fileAtts;

	/* If we are unable to read the attributes, then we set validity to false */
	if ( stat( mFileName.toLatin1().data(), &fileAtts ) != 0 )
		return false;

	off_t tarSize = qlonglong( fileAtts.st_size );

	/* Clear the headers and offsets list */
	headers.clear();
	offsets.clear();

	/* Buffer to store the header */
	char *hdr;

	/* Start reading the archive */
	while ( true ) {

		/* Once we reach the end of file, we exit. tarFile.eof() does not work untill we exceed the position */
		/* Techinically speaking, if the tarfile is intact, we should never reach this point */
		if ( tarFile.tellg() == tarSize )
			return true;

		/* Ready @hdr for reading */
		hdr = new char[ 512 ];

		/* Read the header */
		tarFile.read( hdr, 512 );

		/* Redundant: This should never happen */
		if ( tarFile.gcount() != 512 ) {
			qDebug() << "Error reading" << mFileName;
			return false;
		}

		/* Parse the 512 bytes block which we read, assuming it is a tar header */
		TarHeader *tHdr = TarHeader::parseHeader( QByteArray( hdr, 512 ) );

		/* FUTURE: Set error flag and error string */

		/* Check if the header is valid */
		if ( not tHdr->isValid() ) {
			if ( tarFile.tellg() == tarSize - 512 ) {
				/* This is the last double empty blocks */
				return true;
			}

			else {
				/* This means out tar file is corrupt!! */
				qDebug() << tarFile.tellg();
				qDebug() << "Encountered invalid header, aborting";
				return false;
			}
		}

		/* Store the header for extraction */
		headers << tHdr;

		/* Store the file/dir offset (beginning of the data) */
		offsets << tarFile.tellg();

		if ( tHdr->value( TarHeader::Type ).toChar() == '0' ) {

			/* Get the file size: only if we have file stored */
			long long size = tHdr->value( TarHeader::Size ).toLongLong();

			/* Compute the file buffer */
			int bufLen = 512 - size % 512;

			/* Jump to next header */
			tarFile.seekg( size, std::ios_base::cur );
			if ( bufLen < 512 )
				tarFile.seekg( bufLen, std::ios_base::cur );
		}
	}

	/* We should never reach this point */
	return false;
};

bool TarFile::extract( QString target, QStringList members ) {

	/* Prepare the target for extraction */
	if ( not target.isEmpty() ) {
		if ( not exists( target ) )
			QDir::current().mkpath( target );

		if ( not target.endsWith( "/" ) )
			target += "/";
	}

	/* If headers have not been listed, list them */
	if ( not mRead )
		readArchive();

	/* If @members is not empty, extract only members */
	if ( members.count() ) {
		for( QString member: members ) {
			TarHeader *hdr = getMemberHeader( member );
			if ( hdr->isValid() ) {
				continue;
			}

			else {
				qDebug() << "Skipping non-existent member" <<  member;
			}
		}

		return true;
	}

	for( int i = 0; i < headers.count(); i++ ) {

		/* Get the header */
		TarHeader *hdr = headers.at( i );

		/* Set the corresponding offset */
		tarFile.seekg( offsets.at( i ), std::ios_base::beg );

		/* Get the target filename */
		QString fName = target + hdr->value( TarHeader::Name ).toString();

		switch( hdr->value( TarHeader::Type ).toChar().toLatin1() ) {
			/* Regular file */
			case '0' : {
				/* Get the target file size */
				off_t fSize = hdr->value( TarHeader::Size ).toLongLong();

				/* Create the C++ file object */
				std::ofstream f;

				/* Create the underlying path */
				QDir( target ).mkpath( dirName( hdr->value( TarHeader::Name ).toString() ) );

				/* Open the C++ File Object */
				f.open( fName.toLatin1().data(), std::ios_base::out | std::ios_base::binary );

				/* Buffer to read the file into */
				char *buf;

				/* If the file is smaller than BUFSIZ read it at once */
				if ( fSize < BUFSIZ ) {
					buf = new char[ fSize ];
					tarFile.read( buf, fSize );

					f.write( buf, fSize );
					f.close();
				}

				/* The File is larger than BUFSIZ, read it chunk by chunk */
				else {
					/* Bytes remaining to be read */
					off_t remaining = fSize;

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
				chmod( fName.toLatin1().data(), hdr->value( TarHeader::Mode ).toLongLong() );

				/* Set the dir property: mode */
				struct utimbuf times;
				times.actime = hdr->value( TarHeader::Time ).toLongLong();
				times.modtime = hdr->value( TarHeader::Time ).toLongLong();
				utime( fName.toLatin1().data(), &times );

				break;
			}

			/* SymLink */
			case '2' : {
				/* Create the underlying path */
				QDir( target ).mkpath( dirName( hdr->value( TarHeader::Name ).toString() ) );

				QString path = hdr->value( TarHeader::Target ).toString();
				symlink( path.toLatin1().data(), fName.toLatin1().data() );

				break;
			}

			/* Directory */
			case '5' : {
				/* Create the directory */
				QDir( target ).mkpath( hdr->value( TarHeader::Name ).toString() );

				/* Set the dir property: mode */
				chmod( fName.toLatin1().data(), hdr->value( TarHeader::Mode ).toLongLong() );

				/* Set the dir property: time */
				struct utimbuf times;
				times.actime = hdr->value( TarHeader::Time ).toLongLong();
				times.modtime = hdr->value( TarHeader::Time ).toLongLong();
				utime( fName.toLatin1().data(), &times );

				break;
			}

			/* For others do nothing: until we learn how to create the respective objects: FIFO, BLOCKDEV, CHARDEV etc */
			default: {

				break;
			}
		}
	}

	return true;
};

void TarFile::addFile( QString fn ) {

	/* If the file is readable, and not already added */
	if ( isReadable( fn ) and ( not files.contains( fn ) ) )
		files << QDir::cleanPath( fn );

	else
		qDebug() << "Skipping unreadable file" << fn.toLatin1().data();
};

void TarFile::addDir( QString dn ) {

	dn = QDir::cleanPath( dn );

	if ( not isReadable( dn ) )
		qDebug() << "Skipping unreadable directory" << dn.toLatin1().data();


	if ( not dn.endsWith( "/" ) )
		dn += "/";

	if ( not files.contains( dn ) )
		files << dn;

	DIR *dir;
	struct dirent *ent;
	dir = opendir( dn.toLatin1().data() );

	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {
			/*
				*
				* Do not show . and ..
				*
			*/
			QString nodeName = QString::fromLocal8Bit( ent->d_name );
			if ( ( nodeName.compare( "." ) == 0 ) or ( nodeName.compare( ".." ) == 0 ) )
				continue;

			if( not files.contains( dn + nodeName ) )
				files << dn + nodeName;

			if ( ent->d_type == DT_DIR )
				addDir( dn + nodeName );
		}

		closedir( dir );
	}
};

bool TarFile::writeArchive() {

	switch( mFilter ) {
		case TarFile::XZFilter: {

			return writeArchiveXZ();
		}

		case TarFile::GZFilter: {

			return writeArchiveGZ();
		}

		case TarFile::BZ2Filter: {

			qDebug() << "Using BZ2Filterto write tar.";
			return writeArchiveBZ2();
		}

		case TarFile::NOFilter: {

			/* Open the file for IO */
			tarFile.open( mFileName.toLatin1().data(), std::ios_base::out | std::ios_base::binary );

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
					infile.open( fn.toLatin1().data() );
					while( not infile.eof() ) {
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

			return true;
		}
	}

	return false;
};

void TarFile::close() {

	if ( tarFile.is_open() ) {
		tarFile.flush();
		tarFile.close();
	}
};

TarHeader* TarFile::getMemberHeader( QString member ) {

	/* If @member exists, return the corresponding header */
	for( TarHeader *h: headers ) {
		if ( h->value( TarHeader::Name ).toString() == member )
			return h;
	}

	/* Create a dummy header and return it */
	return TarHeader::createHeader( QByteArray( 512, '\0' ) );
};

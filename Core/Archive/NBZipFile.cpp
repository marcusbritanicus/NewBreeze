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

#include <NBZipFile.hpp>

ZipFile::ZipFile( QString fileName, ZipFile::OpenMode mode ) {

	switch( mode ) {
		case ZipFile::Read: {
			/* Flag to check if the archive has been read once */
			mRead = false;

			/* Set the mFileName value */
			mFileName = fileName;

			/* Set the mMode value */
			mMode = mode;

			/* Open the file for IO */
			arc = zip_open( fileName.toLocal8Bit().data(), 0, NULL );

			break;
		}

		case ZipFile::Write: {
			/* Set the mFileName value */
			mFileName = fileName;

			/* Set the mMode value */
			mMode = mode;

			/* Open the file for IO */
			arc = zip_open( fileName.toLocal8Bit().data(), ZIP_CREATE | ZIP_TRUNCATE, NULL );

			break;
		}
	}
};

bool ZipFile::isOpen() {

	if ( arc )
		return true;

	return false;
};

bool ZipFile::readArchive() {

	mRead = true;
	int numOfEntries = zip_get_num_entries( arc, ZIP_FL_UNCHANGED );

	if ( numOfEntries < 1 ) {

		qDebug() << "This archive contains no members";
		return false;
	}

	for( int i = 0; i < numOfEntries; i++ )
		members << zip_get_name( arc, i, 0 );

	/* If we have reached here, then all is well */
	return true;
};

bool ZipFile::extract( QString target, QStringList memberList ) {

	qDebug() << "Extracting";

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
	if ( memberList.count() ) {
		/* Structure for the openeing compressed file */
		struct zip_file *zf;

		/* Output file */
		std::ofstream ofile;

		/* Iterate through the nembers and extract them all */
		for( QString member: memberList ) {
			/* Open the compressed file inside the zip archive */
			zf = zip_fopen( arc, member.toLocal8Bit().data(), ZIP_FL_UNCHANGED );

			/* Directory entries end with '/', just create the directory */
			if ( member.endsWith( "/" ) )
				QDir( target ).mkpath( member );

			/* Otherwise, create the folder structure */
			else
				QDir( target ).mkpath( dirName( member ) );

			/* Open the output file */
			QString outFn = target + member;
			ofile.open( outFn.toLocal8Bit().data(), std::ios_base::binary );

			/* Output buffer */
			char *buffer;

			/* Number of bytes read by zip_fread */
			int bytesRead = 0;

			/* Read the compressed data until the zip file is read completely */
			do {

				/* Initialize the buffer */
				buffer = new char[ BUFSIZ ];

				/* Read the uncompressed data into the buffer */
				bytesRead = zip_fread( zf, buffer, sizeof( buffer ) );

				/* If some data was uncompressed, write it */
				if ( bytesRead > 0 )
					ofile.write( buffer, bytesRead );

			} while ( bytesRead > 0 );
		}

		return true;
	}

	/* Extracting all the files */

	/* Structure for the openeing compressed file */
	struct zip_file *zf;

	/* Output file */
	std::ofstream ofile;

	/* Iterate through the nembers and extract them all */
	for( QString member: members ) {
		/* Open the compressed file inside the zip archive */
		zf = zip_fopen( arc, member.toLocal8Bit().data(), ZIP_FL_UNCHANGED );

		/* Create the folder structure */
		QDir( target ).mkpath( dirName( member ) );

		/* Open the output file */
		QString outFn = target + member;
		ofile.open( outFn.toLocal8Bit().data(), std::ios_base::binary );
		qDebug() << outFn;

		/* Output buffer */
		char *buffer;

		/* Number of bytes read by zip_fread */
		int bytesRead = 0;

		/* Read the compressed data until the zip file is read completely */
		do {

			/* Initialize the buffer */
			buffer = new char[ BUFSIZ ];

			/* Read the uncompressed data into the buffer */
			bytesRead = zip_fread( zf, buffer, sizeof( buffer ) );

			/* If some data was uncompressed, write it */
			if ( bytesRead > 0 )
				ofile.write( buffer, bytesRead );

		} while ( bytesRead > 0 );

		/* Flush the output file */
		ofile.flush();

		/* Close the output file */
		ofile.close();
	}

	return true;
};

void ZipFile::addFile( QString fn ) {

	/* If the file is readable, and not already added */
	if ( isReadable( fn ) and ( not files.contains( fn ) ) )
		files << QDir::cleanPath( fn );

	else
		qDebug() << "Skipping unreadable file" << fn.toLocal8Bit().data();
};

void ZipFile::addDir( QString dn ) {

	dn = QDir::cleanPath( dn );

	if ( not isReadable( dn ) )
		qDebug() << "Skipping unreadable directory" << dn.toLocal8Bit().data();


	if ( not dn.endsWith( "/" ) )
		dn += "/";

	DIR *dir;
	struct dirent *ent;
	dir = opendir( dn.toLocal8Bit().data() );

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

			if( ( not files.contains( dn + nodeName ) ) and ( ent->d_type == DT_REG ) )
				files << dn + nodeName;

			if ( ent->d_type == DT_DIR )
				addDir( dn + nodeName );
		}

		closedir( dir );
	}
};

bool ZipFile::writeArchive() {

	struct zip_source *zs;
	for( QString file: files ) {
		/* Create the zip_source_files structure */
		zs = zip_source_file( arc, file.toLocal8Bit().data(), 0, 0 );

		/* Sanitize the name: already "QDir::cleanPath" has been performed */
		QString arcFn = file.replace( "../", "" ).replace( "./", "" );
		if ( arcFn.startsWith( "/") )
			arcFn.remove( 0, 1 );

		/* Write the file into the archive */
		if ( zip_file_add( arc, arcFn.toLocal8Bit().data(), zs, 0 ) < 0 ) {
			qDebug() << "Error adding file to the archive";
			qDebug() << zip_strerror( arc );
			zip_close( arc );
			return false;
		}
	}

	/* Clear the files list */
	files.clear();

	return true;
};

void ZipFile::close() {

	if ( arc )
		zip_close( arc );
};

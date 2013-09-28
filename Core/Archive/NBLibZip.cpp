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
#include <NBLibZip.hpp>
#include <NBTools.hpp>

QString NBZip::name = QString();

NBZip::NBZip( QString path, NBZip::Mode openmode) {

	name = QString( path );
	NBZip::Mode mode = openmode;

	switch( mode ) {
		case NBZip::READ : {
			zipArc = zip_open( qPrintable( path ), 0, &NBZipError );
			break;
		}

		case NBZip::WRITE : {
			zipArc = zip_open( qPrintable( path ), ZIP_CREATE, &NBZipError );
			break;
		}
	}
};

void NBZip::create( QStringList entries, QString wDir ) {

	foreach( QString entry, entries ) {
		QFileInfo info( entry );
		if ( info.exists() ) {
			if ( info.isDir() ) {
				fileList.append( recDirWalk( entry ) );
			}

			else if ( info.isFile() )
				fileList.append( entry );
		}
	}

	if ( fileList.size() < 1 )
		return;

	foreach( QString entry, fileList ) {
		QString zipEntry = QString( entry ).remove( wDir );
		if ( zipEntry.startsWith( "/" ) )
			zipEntry.remove( 0, 1 );

		if ( QFileInfo( entry ).isDir() ) {
			zip_add_dir( zipArc, qPrintable( zipEntry ) );
		}

		else {
			struct zip_source *zipSrc = zip_source_file( zipArc, qPrintable( entry ), 0, 0 );
			zip_add( zipArc, qPrintable( zipEntry ), zipSrc );
		}
	}

	zip_close( zipArc );
}

void NBZip::extract( QString destDir ) {

	int numOfEntries = zip_get_num_files( zipArc );

	if ( numOfEntries < 0 )
		return;

	for( int i = 0; i < numOfEntries; i++ ) {

		QString member = QString( zip_get_name( zipArc, i, ZIP_FL_UNCHANGED ) );
		if ( member.endsWith( "/" ) )
			QDir( destDir ).mkpath( member );

		else {

			QString subdir = QFileInfo( member ).path();
			QDir( destDir ).mkpath( subdir );

			extractMember( member, destDir );
		}
	}

	zip_close( zipArc );
};

void NBZip::extractMember( QString arcMember, QString destDir ) {

	int n = 0;
	struct zip_file *zipFile = zip_fopen( zipArc, qPrintable( arcMember ), ZIP_FL_UNCHANGED );
	std::ofstream ofile( qPrintable( destDir + QDir::separator() + arcMember ), std::ofstream::binary  );

	while ( true ) {
		char buffer[ 40960 ] = { "\x00" };
		if ( ( n = zip_fread( zipFile, buffer, sizeof( buffer ) ) ) > 0 ) {
			ofile.write( buffer, n );
			ofile.flush();
		}

		else
			break;
	}

	ofile.close();
	zip_fclose( zipFile );
};

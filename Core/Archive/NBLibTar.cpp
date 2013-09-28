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

#include <NBLibTar.hpp>

using namespace std;

TarFile::TarFile( QString name, char mode ){

	switch( mode ) {
		case 'r' :
			filename = QString( name );
			openmode = QChar( mode );
			openTar( O_RDONLY );
			break;

		case 'w' :
			filename = name;
			openmode = mode;
			openTar( O_WRONLY | O_CREAT );
			break;
	}
};

void TarFile::close() {

	if ( openmode == QChar( 'w' ) ) {
		if ( tar_append_eof( tar ) != 0 ) {
			abort();
		}
	}

	if ( tar_close( tar ) != 0 ) {
		abort();
	}
};

void TarFile::extractAll( QString destDir ) {

	tar_extract_all( tar, ( char *)qPrintable( destDir ) );
};

void TarFile::addFile( QString fn, QString wDir ) {

	QString sn = QString( fn ).remove( wDir );
	tar_append_file( tar, ( char *)qPrintable( fn ), ( char *)qPrintable( sn ) );
};

void TarFile::addDir( QString dn, QString wDir ) {

	QString sn = QString( dn ).remove( wDir );
	tar_append_tree( tar, ( char *)qPrintable( dn ), ( char *)qPrintable( sn ) );
};

// Private TarFile/TAR access
void TarFile::openTar( int mode ) {

	if ( tar_open( &tar, (char *)qPrintable( filename ), NULL, mode, 0644, 0 ) == -1 ) {
		qDebug() << "Unable to open tar archive:" << filename << "\nAborting...";
		abort();
	}
};

int TarFile::readNextHeader() {

	if ( th_read( tar ) == 0 ) {
		if ( TH_ISREG( tar ) && tar_skip_regfile( tar ) != 0 )
			return -1;

		return 0;
	}

	return -1;
};

QString NBTar::name = QString();
NBTar::NBTar( QString path, NBTar::Mode openmode) {

	name = QString( path );
	NBTar::Mode mode = openmode;

	switch( mode ) {
		case NBTar::READ : {
			tarfile = new TarFile( path, 'r' );
			break;
		}

		case NBTar::WRITE : {
			tarfile = new TarFile( path, 'w' );
			break;
		}
	}
};

void NBTar::create( QStringList entries, QString workingDir ) {

	foreach( QString entry, entries ) {
		QFileInfo info( entry );
		if ( info.isDir() )
			tarfile->addDir( entry, workingDir );

		else
			tarfile->addFile( entry, workingDir );
	}

	tarfile->close();
};

void NBTar::extract( QString destDir ) {

	tarfile->extractAll( destDir );
	tarfile->close();
};

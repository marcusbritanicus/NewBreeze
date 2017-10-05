/*
    *
    * NBVaultDatabase.hpp - NewBreeze VaultIndex Class
    *
*/

#include "NBVaultDatabase.hpp"
#include <QDebug>
#include <iostream>

NBVaultRecord* NBVaultDatabase::recordForPath( QString path, QByteArray vaultPass ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	/* We remove the leading '/' */
	path.remove( 0, 1 );

	NBVaultRecord *rec = new NBVaultRecord();

	/* Create archive object */
	struct archive *ar = archive_read_new();

	/* Set filter format: zip */
	archive_read_support_format_zip( ar );

	/* Open for reading */
	archive_read_open_filename( ar, vaultDB.toLocal8Bit().constData(), 10240 );

	/* Create archive entry object */
	struct archive_entry *entry = archive_entry_new();

	while( true ) {
		int r = archive_read_next_header( ar, &entry );

		if ( r != ARCHIVE_OK ) {
			rec->encPath = QString();
			rec->type = QString();
			rec->recordPass = QByteArray();
			return rec;
		}

		/* The leading '/' is removed from our implementation of zip while adding files */
		if ( path == archive_entry_pathname( entry ) )
			break;
	}

	int eSize = archive_entry_size( entry );

	NBSalsa20 s20( "", NULL );
	char *buff = new char[ eSize + 1 ];

	int rSize = archive_read_data( ar, buff, eSize );
	QByteArray data( buff, rSize );

	QByteArray info = s20.decryptData( data, vaultPass.constData() );
	QList<QByteArray> infoList = info.split( '\n' );

	rec->encPath = QString::fromLocal8Bit( infoList.at( 0 ) );
	rec->type = QString::fromLocal8Bit( infoList.at( 1 ) );
	rec->recordPass = infoList.at( 2 );

	archive_read_close( ar );
	archive_read_free( ar );

	return rec;
};

bool NBVaultDatabase::addRecordForPath( QString path, NBVaultRecord *rec, QByteArray vaultPass ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	/* Create data from @rec */
	QByteArray data;
	data += rec->encPath + "\n";
	data += rec->type + "\n";
	data += rec->recordPass + "\n";

	/* Encrypt the data */
	NBSalsa20 s20( "", NULL );
	data = s20.encryptData( data, vaultPass.constData() );

	AbZip zip( vaultDB );
	zip.writeFile( path, data, AbZip::AddRelativePaths | AbZip::SkipExistingFiles );
	zip.close();

	if ( zip.errorCount() ) {
		qDebug() << zip.errorString();
		return false;
	}

	return true;
};

bool NBVaultDatabase::removeRecordForPath( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	AbZip zip( vaultDB );
	zip.deleteFile( path );
	zip.close();

	if ( zip.errorCount() ) {
		qDebug() << zip.errorString();
		return false;
	}

	return true;
};

bool NBVaultDatabase::checkVaultPassword( QByteArray vPass ) {

	/* Password hash */
	QByteArray vpHash = QCryptographicHash5::hash( vPass, QCryptographicHash5::Sha3_512 );
	QByteArray data;

	/* Create archive object */
	struct archive *ar = archive_read_new();

	/* Set filter format: zip */
	archive_read_support_format_zip( ar );

	/* Open for reading */
	archive_read_open_filename( ar, vaultDB.toLocal8Bit().data(), 10240 );

	/* Create archive entry object */
	struct archive_entry *entry = archive_entry_new();
	int r = archive_read_next_header( ar, &entry );

	while( r == ARCHIVE_OK ) {

		if ( archive_entry_pathname( entry ) == QString( "passHash" ) ) {

			int eSize = archive_entry_size( entry );
			char *buff = new char[ eSize + 1 ];

			int rSize = archive_read_data( ar, buff, eSize );
			data = QByteArray( buff, rSize );

			break;
		}

		r = archive_read_next_header( ar, &entry );
		if ( r == ARCHIVE_FAILED or r == ARCHIVE_FATAL )
			qDebug() << archive_error_string( ar );
	}

	archive_read_close( ar );
	archive_read_free( ar );

	if ( archive_errno( ar ) )
		return false;

	return ( data == vpHash );
};

bool NBVaultDatabase::changeVaultPassword( QByteArray oldPass, QByteArray newPass ) {

	NBSalsa20 old20( "", NULL );
	NBSalsa20 new20( "", NULL );

	/* Create archive object */
	struct archive *ar = archive_read_new();

	/* Set filter format: zip */
	archive_read_support_format_zip( ar );

	/* Open for reading */
	archive_read_open_filename( ar, vaultDB.toLocal8Bit().data(), 10240 );
	AbZip zip( vaultDB + ".tmp" );

	/* Create archive entry object */
	struct archive_entry *entry = archive_entry_new();

	while( true ) {
		int r = archive_read_next_header( ar, &entry );

		if ( r != ARCHIVE_OK ) {
			qDebug() << "Error changing password. Password not changed";
			return false;
		}

		int eSize = archive_entry_size( entry );
		char *buff = new char[ eSize + 1 ];

		int rSize = archive_read_data( ar, buff, eSize );
		QByteArray data( buff, rSize );

		QByteArray info = old20.decryptData( data, oldPass.constData() );
		QList<QByteArray> infoList = info.split( '\n' );

		QByteArray info2 = new20.encryptData( info, newPass.constData() );
		zip.writeFile( archive_entry_pathname( entry ), info2 );
	}

	archive_read_close( ar );
	archive_read_free( ar );

	/* Write the new password hash */
	zip.writeFile( "passHash", QCryptographicHash5::hash( newPass, QCryptographicHash5::Sha3_512 ), AbZip::AddReplaceFiles );

	zip.close();

	QFile::remove( vaultDB );
	QFile::rename( vaultDB + ".tmp", vaultDB );

	return true;
};

void NBVaultDatabase::testWrite() {

	AbZip zip( "/tmp/vault.db" );
	NBSalsa20 s20( "", NULL );

	char data[ 4501 ] = { 0 };
	for( int i = 0; i < 100; i++ )
		strcat( data, "The quick brown fox jumps over the lazy dog.\n" );

	zip.writeFile( "/home/cosmos/testFile", s20.encryptData( data, "Password" ).data() );
	zip.close();
};

void NBVaultDatabase::testRead() {

	struct archive *ar;
	ar = archive_read_new();
	/* Filter Format */
	archive_read_support_format_zip( ar );

	/* Open for write */
	archive_read_open_filename( ar, "/tmp/vault.db", 10240 );

	struct archive_entry *entry = archive_entry_new();
	archive_read_next_header( ar, &entry );

	NBSalsa20 s20( "", NULL );
	char buff[ 4501 ] = { 0 };

	int rSize = archive_read_data( ar, buff, archive_entry_size( entry ) );
	QByteArray data( buff, rSize );

	qDebug() << s20.decryptData( data, "Password" ).data();

	archive_entry_free( entry );

	archive_read_close( ar );
	archive_read_free( ar );
};

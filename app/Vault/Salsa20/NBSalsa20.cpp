/*
	*
	* NBSalsa20.cpp - NewBreeze File Encryption Class
	*
*/

#include "NBSalsa20.hpp"
#include <iostream>

NBSalsa20::NBSalsa20( QString path, QWidget *parent ) : QObject() {

	/* Parent for PasswordInput dialog */
	mParent = parent;

	/* Full path */
	mFilePath = path;
};

void NBSalsa20::makeIVFromString( QByteArray text, uint8_t *iv ) {

	/* Get the hashed password to be used */
	QByteArray password = QCryptographicHash::hash( text, QCryptographicHash::Md5 );

	for( int i = 0; i < password.size(); i += 4 ) {
		for( int c = 0; c < 4; c++ ) {
			uint8_t value = 0;
			char ch = password.at( c );

			if( ch >= '0' && ch <= '9' )
				value = ch - '0';

			else if( ch >= 'A' && ch <= 'F' )
				value = ch - 'A' + 0x0A;

			else if ( ch >= 'a' && ch <= 'f' )
				value = ch - 'a' + 0x0A;

			iv[ i / 4 ] |= ( value << ( 4 - i * 4 ) );
		}
	}
};

void NBSalsa20::makeKeyFromString( QByteArray text, uint8_t *key ) {

	/* Get the hashed password to be used */
	QByteArray password = QCryptographicHash::hash( text, QCryptographicHash::Md5 );

	for( int i = 0; i < password.size(); i += 2 ) {
		for( int c = 0; c < 2; c++ ) {
			uint8_t value = 0;
			char ch = password.at( c );

			if( ch >= '0' && ch <= '9' )
				value = ch - '0';

			else if( ch >= 'A' && ch <= 'F' )
				value = ch - 'A' + 0x0A;

			else if ( ch >= 'a' && ch <= 'f' )
				value = ch - 'a' + 0x0A;

			key[ i / 2 ] |= ( value << ( 4 - i * 4 ) );
		}
	}
};

void NBSalsa20::encrypt( QByteArray password ) {

	Salsa20 s20;

	/* Initialization Vector: */
	uint8_t iv[ 8 ] = { 0 };
	makeIVFromString( password, iv );

	uint8_t passwd[ 32 ] = { 0 };
	makeKeyFromString( password, passwd );

	s20.setKey( passwd );
	s20.setIv( iv );

	QFile ifile( mFilePath );
	ifile.open( QFile::ReadOnly );

	// if ( exists( mFilePath + ".s20" ) ) {
		// QString on = mFilePath + ".s20";
		// if ( rename( on.toLocal8Bit().data(), ( on  + ".orig" ).toLocal8Bit().data() ) != 0 ) {
			// QMessageBox::information(
				// mParent,
				// "NewBreeze - File Encryption Failed",
				// "I am unable to rename the already existing encrypted file. This may be because you do not have write permission in this directory. "
				// "Please gain sufficient permissions and try again."
			// );

			// return;
		// }
	// }

	QFile ofile( mFilePath + ".s20" );
	ofile.open( QFile::WriteOnly );

	uint8_t *block;
	while ( not ifile.atEnd() ) {
		QByteArray ba = ifile.read( 8192 );
		block = reinterpret_cast<uint8_t*>( ba.data() );

		/* If 8192 bytes were read, then process it as a block */
		if ( ba.length() == 8192 ) {

			s20.processBlocks( block, block, 128 );
			ofile.write( reinterpret_cast<const char*>( block ), 8192 );
		}

		/* Otherwise process it as bytes */
		else {

			s20.processBytes( block, block, ba.length() );
			ofile.write( reinterpret_cast<const char*>( block ), ba.length() );
		}
	}

	ifile.close();
	ofile.close();

	/* Remove the input file */
	ifile.remove();
};

void NBSalsa20::decrypt( QByteArray password ) {

	Salsa20 s20;

	/* Initialization Vector: */
	uint8_t iv[ 8 ] = { 0 };
	makeIVFromString( password, iv );

	uint8_t passwd[ 32 ] = { 0 };
	makeKeyFromString( password, passwd );

	s20.setKey( passwd );
	s20.setIv( iv );

	QFile ifile( mFilePath );
	ifile.open( QFile::ReadOnly );

	/* Output filepath */
	QString oFilePath = mFilePath;
	oFilePath.chop( 4 );

	// if ( exists( oFilePath ) ) {
		// if ( rename( oFilePath.toLocal8Bit().data(), ( oFilePath  + ".orig" ).toLocal8Bit().data() ) != 0 ) {
			// QMessageBox::information(
				// mParent,
				// "NewBreeze - File Encryption Failed",
				// "I am unable to rename the already existing decrypted file. This may be because you do not have write permission in this directory. "
				// "Please gain sufficient permissions and try again."
			// );

			// return;
		// }
	// }

	QFile ofile( oFilePath );
	ofile.open( QFile::WriteOnly );

	uint8_t *block;
	while ( not ifile.atEnd() ) {
		QByteArray ba = ifile.read( 8192 );
		block = reinterpret_cast<uint8_t*>( ba.data() );

		/* If 8192 bytes were read, then process it as a block */
		if ( ba.length() == 8192 ) {

			s20.processBlocks( block, block, 128 );
			ofile.write( reinterpret_cast<const char*>( block ), 8192 );
		}

		/* Otherwise process it as bytes */
		else {

			s20.processBytes( block, block, ba.length() );
			ofile.write( reinterpret_cast<const char*>( block ), ba.length() );
		}
	}

	ifile.close();
	ofile.close();

	/* Remove the input file */
	ifile.remove();
};

QByteArray NBSalsa20::encryptData( QByteArray data, QByteArray password ) {

	Salsa20 s20;

	/* Initialization Vector: */
	uint8_t iv[ 8 ] = { 0 };
	makeIVFromString( password, iv );

	uint8_t passwd[ 32 ] = { 0 };
	makeKeyFromString( password, passwd );

	s20.setKey( passwd );
	s20.setIv( iv );

	uint8_t *block;
	block = reinterpret_cast<uint8_t*>( data.data() );

	s20.processBytes( block, block, data.length() );
	QByteArray ba = QByteArray( reinterpret_cast<const char*>( block ), data.length() );

	return ba;
};

QByteArray NBSalsa20::decryptData( QByteArray data, QByteArray password ) {

	Salsa20 s20;

	/* Initialization Vector: */
	uint8_t iv[ 8 ] = { 0 };
	makeIVFromString( password, iv );

	uint8_t passwd[ 32 ] = { 0 };
	makeKeyFromString( password, passwd );

	s20.setKey( passwd );
	s20.setIv( iv );

	uint8_t *block;
	block = reinterpret_cast<uint8_t*>( data.data() );

	s20.processBytes( block, block, data.length() );
	QByteArray ba = QByteArray( reinterpret_cast<const char*>( block ), data.length() );

	return ba;
};

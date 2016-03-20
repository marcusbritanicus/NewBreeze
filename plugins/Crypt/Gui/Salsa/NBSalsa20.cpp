/*
	*
	* NBSalsa20.cpp - NewBreeze File Encryption Class
	*
*/

#include <NBSalsa20.hpp>

NBSalsa20::NBSalsa20( QString path, QWidget *parent ) : QObject() {

	/* Parent for PasswordInput dialog */
	mParent = parent;

	/* Full path */
	mFilePath = path;
};

void NBSalsa20::makeIVFromString( QString text, uint8_t *iv ) {

	/* Get the hashed password to be used */
	QString password;
	#if QT_VERSION >= 0x050100
		password = QCryptographicHash::hash( text.toUtf8(), QCryptographicHash5::Md5 );
	#else
		password = QCryptographicHash5::hash( text.toUtf8(), QCryptographicHash5::Md5 );
	#endif

	for( int i = 0; i < password.size(); i += 4 ) {
		for( int c = 0; c < 4; c++ ) {
			uint8_t value = 0;
			char ch = password.at( i ).toLatin1();

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

void NBSalsa20::makeKeyFromString( QString text, uint8_t *key ) {

	/* Get the hashed password to be used */
	QString password;
	#if QT_VERSION >= 0x050100
		password = QCryptographicHash::hash( text.toUtf8(), QCryptographicHash5::Md5 );
	#else
		password = QCryptographicHash5::hash( text.toUtf8(), QCryptographicHash5::Md5 );
	#endif

	for( int i = 0; i < password.size(); i += 2 ) {
		for( int c = 0; c < 2; c++ ) {
			uint8_t value = 0;
			char ch = password.at( i ).toLatin1();

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

void NBSalsa20::encrypt() {

	NBPasswordDialog *pDlg = new NBPasswordDialog( false, qobject_cast<QWidget*>( parent() ) );
	Salsa20 s20;
	if ( pDlg->exec() == QDialog::Accepted ) {

		/* Get the password */
		QString password = pDlg->password();
		pDlg->clear();

		/* Initialization Vector: */
		uint8_t iv[ 8 ] = { 0 };
		makeIVFromString( password, iv );

		/* Get the hashed password to be used */
		#if QT_VERSION >= 0x050100
			password = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
		#else
			password = QCryptographicHash5::hash( password.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
		#endif

		uint8_t passwd[ 32 ] = { 0 };
		makeKeyFromString( password, passwd );

		s20.setKey( passwd );
		s20.setIv( iv );

		QFile ifile( mFilePath );
		ifile.open( QFile::ReadOnly );

		if ( exists( mFilePath + ".s20" ) ) {
			QString on = mFilePath + ".s20";
			if ( rename( on.toLocal8Bit().data(), ( on  + ".orig" ).toLocal8Bit().data() ) != 0 ) {
				QMessageBox::information(
					mParent,
					"NewBreeze - File Encryption Failed",
					"I am unable to rename the already existing encrypted file. This may be because you do not have write permission in this directory. "
					"Please gain sufficient permissions and try again."
				);

				return;
			}
		}

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
	}
};

void NBSalsa20::decrypt() {

	PasswordInput *pInput = new PasswordInput( qobject_cast<QWidget*>( parent() ) );
	Salsa20 s20;
	if ( pInput->exec() == QDialog::Accepted ) {

		/* Get the password */
		QString password = pInput->password();
		pInput->clear();

		/* Initialization Vector: */
		uint8_t iv[ 8 ] = { 0 };
		makeIVFromString( password, iv );

		/* Get the hashed password to be used */
		#if QT_VERSION >= 0x050100
			password = QCryptographicHash::hash( password.toUtf8(), QCryptographicHash5::Sha3_256 );
		#else
			password = QCryptographicHash5::hash( password.toUtf8(), QCryptographicHash5::Sha3_256 );
		#endif

		uint8_t passwd[ 32 ] = { 0 };
		makeKeyFromString( password, passwd );

		s20.setKey( passwd );
		s20.setIv( iv );

		QFile ifile( mFilePath );
		ifile.open( QFile::ReadOnly );

		/* Output filepath */
		QString oFilePath = mFilePath;
		oFilePath.chop( 4 );

		if ( exists( oFilePath ) ) {
			if ( rename( oFilePath.toLocal8Bit().data(), ( oFilePath  + ".orig" ).toLocal8Bit().data() ) != 0 ) {
				QMessageBox::information(
					mParent,
					"NewBreeze - File Encryption Failed",
					"I am unable to rename the already existing decrypted file. This may be because you do not have write permission in this directory. "
					"Please gain sufficient permissions and try again."
				);

				return;
			}
		}

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
	}
};

void NBSalsa20::changePass() {

	QMessageBox::warning(
		mParent,
		"NewBreeze - Warning",
		"<p>Please note that I have <b>no way of validating the current password.</b> Due to the structure of this cipher, "
		"it will just process the file irrespective of the password.</p>"
		"<p>Inputting the <b>wrong password</b> as your current password could <b>destroy the contents</b> of your file "
		"forever. Kindly proceed with <b>extreme caution</b>.</p>"
		"<p><center><b><font color='red'>You are hereby warned.</font</b></center></p>"
	);

	NBPasswordDialog *pDlg = new NBPasswordDialog( true, qobject_cast<QWidget*>( parent() ) );

	/* By pass the true password validation, as the password cannot be validated */
	connect( pDlg, SIGNAL( oldPassword( QString ) ), this, SLOT( validateOldPassword() ) );
	connect( this, SIGNAL( proceed( bool ) ), pDlg, SLOT( setValidated( bool ) ) );

	if ( pDlg->exec() != QDialog::Accepted )
		return;

	/* Get the passwords */
	QString oPassword = pDlg->oldPassword();
	QString nPassword = pDlg->password();
	pDlg->clear();

	/* Create the Salsa Cipher objects */
	Salsa20 decoder;
	Salsa20 encoder;

	/* Setup old Salsa20 Cipher */
	uint8_t ivOld[ 8 ] = { 0 };
	makeIVFromString( oPassword, ivOld );

	/* Get the hashed password to be used */
	#if QT_VERSION >= 0x050100
		oPassword = QCryptographicHash::hash( oPassword.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
	#else
		oPassword = QCryptographicHash5::hash( oPassword.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
	#endif

	uint8_t passwdOld[ 32 ] = { 0 };
	makeKeyFromString( oPassword, passwdOld );

	decoder.setKey( passwdOld );
	decoder.setIv( ivOld );
	/* ------------------------ */

	/* Setup New Salsa20 Cipher */
	uint8_t ivNew[ 8 ] = { 0 };
	makeIVFromString( nPassword, ivNew );

	/* Get the hashed password to be used */
	#if QT_VERSION >= 0x050100
		nPassword = QCryptographicHash::hash( nPassword.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
	#else
		nPassword = QCryptographicHash5::hash( nPassword.toUtf8(), QCryptographicHash5::Sha3_256 ).toHex();
	#endif

	uint8_t passwdNew[ 32 ] = { 0 };
	makeKeyFromString( nPassword, passwdNew );

	encoder.setKey( passwdNew );
	encoder.setIv( ivNew );
	/* ------------------------ */

	QFile ifile( mFilePath );
	if ( not ifile.open( QFile::ReadOnly ) ) {
		QMessageBox::information(
			mParent,
			"NewBreeze - File Encryption Failed",
			"I am unable to open the encrypted file for reading. This may be because you do not have read permission in this directory. "
			"Please gain sufficient permissions and try again."
		);

		return;
	}

	/* Output filepath */
	QString oFilePath = mFilePath + QDateTime::currentDateTime().toString( ".yyyyMMddThhmmss" );

	QFile ofile( oFilePath );
	if ( not ofile.open( QFile::WriteOnly ) ) {
		QMessageBox::information(
			mParent,
			"NewBreeze - File Encryption Failed",
			"I am unable to open the encrypted file for writing. This may be because you do not have write permission in this directory. "
			"Please gain sufficient permissions and try again."
		);

		return;
	}

	uint8_t *block;
	while ( not ifile.atEnd() ) {
		QByteArray ba = ifile.read( 8192 );
		block = reinterpret_cast<uint8_t*>( ba.data() );

		/* If 8192 bytes were read, then process it as a block */
		if ( ba.length() == 8192 ) {

			decoder.processBlocks( block, block, 128 );
			encoder.processBlocks( block, block, 128 );
			ofile.write( reinterpret_cast<const char*>( block ), 8192 );
		}

		/* Otherwise process it as bytes */
		else {

			decoder.processBytes( block, block, ba.length() );
			encoder.processBytes( block, block, ba.length() );
			ofile.write( reinterpret_cast<const char*>( block ), ba.length() );
		}
	}

	ifile.close();
	ofile.close();

	unlink( mFilePath.toLocal8Bit().data() );
	rename( oFilePath.toLocal8Bit().data(), mFilePath.toLocal8Bit().data() );
};

void NBSalsa20::validateOldPassword() {

	qDebug() << "Validating password:" << "[Done]";
	qDebug() << "Firing proceed( true )";
	emit proceed( true );
};

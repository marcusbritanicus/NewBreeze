/*
    *
    * NBVault.hpp - NewBreeze Vault Header
    *
*/

#include "NBVault.hpp"
#include "NBPasswordDialog.hpp"
#include "NBPasswordInput.hpp"
#include "EncFS/NBEncFS.hpp"

QByteArray NBVault::vaultPass = QByteArray();
NBVault::Options NBVault::mKeyStoreOption = NBVault::AskVaultKeyEverytime;

NBVault *NBVault::vault = NULL;

NBVault::NBVault() {

	if ( not vaultDB.value( "Password" ).toByteArray().size() ) {
		QMessageBox::information(
			NULL,
			"NewBreeze Vault",
			"It seems that this is the first time you are using NewBreeze Vault. Please set the vault password."
		);

		NBPasswordDialog *pDlg = new NBPasswordDialog( NULL );
		if ( pDlg->exec() ) {
			vaultDB.setValue( "Password", QCryptographicHash5::hash( pDlg->password(), QCryptographicHash5::Sha3_512 ) );

			init = true;
		}
	}

	mKeyStoreOption = NBVault::AskVaultKeyEverytime;
};

NBVault* NBVault::instance() {

	if ( not vault or vault->init )
		vault = new NBVault();

	return vault;
};

void NBVault::setVaultOption( NBVault::Options opt ) {

	mKeyStoreOption = opt;
};

bool NBVault::decryptFile( QString path ) {

	QByteArray pass = vaultPassword();
	if ( not pass.count() )
		return false;

	if ( path.endsWith( ".s20" ) )
		path.chop( 4 );

	NBVaultRecord *record = NBVaultDatabase::recordForPath( path, pass );
	if ( not record->recordPass.size() )
		return false;

	if ( not exists( path + ".s20" ) )
		return false;

	NBVaultDatabase::removeRecordForPath( path );
	if ( not record->recordPass.size() )
		return false;

	NBSalsa20 s20( record->encPath );
	s20.decrypt( record->recordPass );

	return true;
};

bool NBVault::encryptFile( QString path ) {

	QByteArray pass = vaultPassword();
	if ( not pass.count() )
		return false;

	if ( not isFile( path ) )
		return false;

	NBVaultRecord *record = new NBVaultRecord();
	record->encPath = path + ".s20";
	record->type = QString( "file" );
	record->recordPass = generatePassword();

	NBSalsa20 s20( path );
	s20.encrypt( record->recordPass );

	NBVaultDatabase::addRecordForPath( path, record, pass );

	return true;
};

bool NBVault::decryptDirectory( QString path ) {

	QByteArray pass = vaultPassword();
	if ( not pass.count() )
		return false;

	NBVaultRecord *record = NBVaultDatabase::recordForPath( path, pass );
	if ( not record->recordPass.size() )
		return false;

	NBEncFS encfs( record->encPath, path );
	return encfs.mountDir( QString::fromLocal8Bit( record->recordPass.toHex() ) );
};

bool NBVault::encryptDirectory( QString path ) {

	if ( NBVaultDatabase::isEncryptedLocation( path ) ) {
		NBEncFS encfs( QString( " " ), path );
		return encfs.unmountDir();
	}

	else {
		QByteArray pass = vaultPassword();
		if ( not pass.count() )
			return false;

		NBVaultRecord *record = new NBVaultRecord();
		record->encPath = dirName( path ) + "." + baseName( path ) + ".enc";
		record->type = QString( "dir" );
		record->recordPass = generatePassword();

		NBEncFS encfs( record->encPath, path );
		if ( encfs.createEncFS( record->recordPass.toHex() ) )
			return NBVaultDatabase::addRecordForPath( path, record, pass );

		else
			return false;
	}
};

bool NBVault::isDirectoryDecrypted( QString path ) {

	return false;
};

QByteArray NBVault::vaultPassword() {

	if ( ( mKeyStoreOption == NBVault::StoreVaultKeyForSession ) and ( vaultPass.count() ) )
		return vaultPass;

	NBPasswordInput *pIn = new NBPasswordInput( NULL );
	pIn->exec();

	QByteArray passwd = pIn->password().toLocal8Bit();
	pIn->clear();

	if ( not NBVaultDatabase::checkVaultPassword( passwd ) ) {
		QMessageBox::information( NULL, "NewBreeze Vault Error", "You have entered the wrong password!" );
		return QByteArray();
	}

	else {
		if ( mKeyStoreOption == NBVault::StoreVaultKeyForSession )
			vaultPass = passwd;

		return passwd;
	}
};

bool NBVault::changeVaultPassword() {

	QByteArray oldPass = vaultPassword();
	QByteArray newPass;

	NBPasswordDialog *pDlg = new NBPasswordDialog( NULL );
	if ( pDlg->exec() ) {
		newPass = pDlg->password();
		vaultDB.setValue( "Password", QCryptographicHash5::hash( pDlg->password(), QCryptographicHash5::Sha3_512 ) );

		return NBVaultDatabase::changeVaultPassword( oldPass, newPass );
	}

	return false;
};

QByteArray NBVault::generatePassword() {

	FILE *randF = fopen( "/dev/urandom", "r" );
	char randData[ 1025 ] = { 0 };
	fread( randData, 1, 1024, randF );
	fclose( randF );

	return QCryptographicHash5::hash( QByteArray( randData, 1024 ), QCryptographicHash5::Sha3_512 );
};

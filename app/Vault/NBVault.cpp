/*
    *
    * NBVault.hpp - NewBreeze Vault Class
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

	init = true;
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
		NBVault *vlt = NBVault::instance();
		QByteArray pass = vlt->vaultPassword();
		if ( not pass.count() )
			return false;

		QString mtPt = baseName( path );

		NBVaultRecord *record = new NBVaultRecord();
		record->encPath = dirName( path ) + "." + mtPt;
		record->type = QString( "dir" );
		record->recordPass = vlt->generatePassword();

		NBEncFS encfs( record->encPath, path );
		if ( encfs.createEncFS( record->recordPass.toHex() ) )
			return NBVaultDatabase::addRecordForPath( path, record, pass );

		else
			return false;
	}
};

bool NBVault::isDirectoryDecrypted( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	QProcess proc;
	proc.start( "df -h" );
	proc.waitForFinished();

	QString output = QString::fromLocal8Bit( proc.readAllStandardOutput() );

	if ( output.contains( " " + path + "\n" ) )
		return true;

	return false;
};

QByteArray NBVault::vaultPassword() {

	if ( not vaultDB.value( "Password" ).toByteArray().size() ) {
		QMessageBox *msgBox = new QMessageBox( NULL );
		msgBox->setWindowTitle( "NewBreeze - NBVault" );
		msgBox->setIcon( QMessageBox::Information );
		msgBox->setText( "NewBreeze Vault First Use" );
		msgBox->setInformativeText(
			"<p>It seems that this is the first time you are using NewBreeze Vault. Please read the following disclaimer carefully.</p>"
			"<p><i>This implementation of NBVault is given to you as is without any without any warranty under the terms of the "
			"GNU General Public License (version 3 or later). This implementation is <b>NOT CERTIFIED</b> by any cryptography expert. "
			"NBVault is meant for keeping your private/sensitive stuff from prying eyes and not for secure storage of important data.</i></p>"
			"<p>Please accept that you are using NBVault at your own risk and the developer is not responsible for any data loss.</p>"
		);
		msgBox->addButton( "&Accept", QMessageBox::AcceptRole );
		msgBox->addButton( "&Reject", QMessageBox::RejectRole );

		if ( msgBox->exec() == QMessageBox::RejectRole )
			return QByteArray();

		QMessageBox::information( NULL, "Newbreeze - NBVault", "Please set the vault password and remember it. If the vault password is forgotten, "
		"you <b>cannot</b> retrieve the encrypted data from files/folders. You may set a pattern or use a text password." );

		NBPasswordDialog *pDlg = new NBPasswordDialog( NULL );
		if ( pDlg->exec() )
			vaultDB.setValue( "Password", QCryptographicHash::hash( pDlg->password(), QCryptographicHash::Sha3_512 ) );
	}

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
		vaultDB.setValue( "Password", QCryptographicHash::hash( pDlg->password(), QCryptographicHash::Sha3_512 ) );

		return NBVaultDatabase::changeVaultPassword( oldPass, newPass );
	}

	return false;
};

QByteArray NBVault::generatePassword() {

	FILE *randF = fopen( "/dev/urandom", "r" );
	char randData[ 1025 ] = { 0 };
	fread( randData, 1, 1024, randF );
	fclose( randF );

	return QCryptographicHash::hash( QByteArray( randData, 1024 ), QCryptographicHash::Sha3_512 );
};

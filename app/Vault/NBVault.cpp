/*
    *
    * NBVault.hpp - NewBreeze Vault Header
    *
*/

#include "NBVault.hpp"
#include "NBPasswordDialog.hpp"
#include "NBPasswordInput.hpp"

QByteArray NBVault::vaultPass = QByteArray();
NBVault::Options NBVault::mKeyStoreOption = NBVault::AskVaultKeyEverytime;

NBVault *NBVault::vault = NULL;

NBVault::NBVault() {

	if ( not exists( vaultDB ) ) {
		QMessageBox::information(
			NULL,
			"NewBreeze Vault",
			"It seems that this is the first time you are using NewBreeze Vault. Please set the vault password."
		);

		NBPasswordDialog *pDlg = new NBPasswordDialog( NULL );
		if ( pDlg->exec() ) {
			AbZip zip( vaultDB);
			zip.writeFile( "passHash", QCryptographicHash5::hash( pDlg->password(), QCryptographicHash5::Sha3_512 ) );
			zip.close();

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

	NBVaultRecord *record = NBVaultDatabase::recordForPath( path, pass );
	Q_UNUSED( record );

	qDebug() << record->encPath;
	qDebug() << record->type;
	qDebug() << record->recordPass;

	return false;
};

bool NBVault::decryptDirectory( QString path ) {

	QByteArray pass = vaultPassword();
	if ( not pass.count() )
		return false;

	NBVaultRecord *record = NBVaultDatabase::recordForPath( path, pass );
	Q_UNUSED( record );

	return false;
};

QByteArray NBVault::vaultPassword() {

	if ( ( mKeyStoreOption == NBVault::StoreVaultKeyForSession ) and ( vaultPass.count() ) )
		return vaultPass;

	NBPasswordInput *pIn = new NBPasswordInput( NULL );
	pIn->exec();

	vaultPass = pIn->password().toLocal8Bit();
	pIn->clear();

	if ( not NBVaultDatabase::checkVaultPassword( vaultPass ) ) {
		QMessageBox::information( NULL, "NewBreeze Vault Error", "You have entered the wrong password!" );
		vaultPass = QByteArray();
	}

	return vaultPass;
};

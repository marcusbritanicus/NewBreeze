/*
    *
    * NBVaultDatabase.hpp - NewBreeze VaultIndex Header
    *
*/

#pragma once

#ifdef STANDALONE
	#include "Global2.hpp"
#else
	#include "Global.hpp"
#endif
#include "Salsa20/NBSalsa20.hpp"

typedef struct _NBVaultRecord {
	QString encPath;
	QString type;
	QByteArray recordPass;
} NBVaultRecord;

static QSettings vaultDB( QDir( NBXdg::userDir( NBXdg::XDG_CONFIG_HOME ) ).filePath( "NewBreeze/Vault.db" ), QSettings::NativeFormat );

namespace NBVaultDatabase {

	bool isEncryptedLocation( QString path );

	/* NBVaultRecord object for a given path */
	NBVaultRecord* recordForPath( QString path, QByteArray vaultPass );

	/* Add a record to the database */
	bool addRecordForPath( QString path, NBVaultRecord *, QByteArray vaultPass );

	/* Remove a record to the database */
	bool removeRecordForPath( QString path );

	/* Check if the vault key is correct */
	bool checkVaultPassword( QByteArray vPass );

	/* Change password of the Vault */
	bool changeVaultPassword( QByteArray oldPass, QByteArray newPass );
};

/*
    *
    * NBVaultDatabase.hpp - NewBreeze VaultIndex Header
    *
*/

#pragma once

/* Local */
#include "Global.hpp"
#include "Salsa20/NBSalsa20.hpp"
#include "MicroDB/AbZip.h"

typedef struct _NBVaultRecord {
	QString encPath;
	QString type;
	QByteArray recordPass;
} NBVaultRecord;

static QString vaultDB = QDir( NBXdg::userDir( NBXdg::XDG_CONFIG_HOME ) ).filePath( "NewBreeze/Vault.db" );

namespace NBVaultDatabase {

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

	/* Test functions: read and write encrypted dummy */
	void testRead();
	void testWrite();
};

/*
    *
    * NBVault.hpp - NewBreeze Vault Header
    *
*/

#pragma once

#include "Global.hpp"
#include "NBVaultDatabase.hpp"

class NBVault {

	public:
		enum Options {
			StoreVaultKeyForSession = 0x39F064,					// Store the vault key in memory
			AskVaultKeyEverytime								// Ask the key from the user everytime
		};

		static NBVault* instance();

		static void setVaultOption( NBVault::Options opt );

		/* Decrypt and view/edit file */
		bool decryptFile( QString path );

		/* Encrypt and store the file */
		bool encryptFile( QString path );

		/* Decrypt and mount the directory at @path */
		bool decryptDirectory( QString path );

		/* Unmount and encrypt the directory at @path */
		bool encryptDirectory( QString path );

		/* Get vault pass */
		QByteArray vaultPassword();

		/* Change Vault Password */
		bool changeVaultPassword();

	private:
		NBVault();

		static NBVault* vault;
		bool init;

		static QByteArray vaultPass;
		static NBVault::Options mKeyStoreOption;
};

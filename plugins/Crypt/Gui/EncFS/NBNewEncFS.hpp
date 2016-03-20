/*
	*
	* NBNewEncFS.hpp - New EncFS Volume Class header
	*
*/

#pragma once

#include <Global.hpp>
#include <NBPasswordEdit.hpp>
#include <NBPasswordInput.hpp>
#include <NBTools.hpp>

class NBNewEncFS : public QDialog {
	Q_OBJECT

	public:
		NBNewEncFS( QString encPath, QString decPath, QWidget *parent = NULL );

		/* Show the dialog */
		int show();

		/* Encrypted directoryp path */
		QString encryptedPath();

		/* Decrypted directoryp path */
		QString decryptedPath();

		/* Password */
		QString password();

	private:
		void createGUI();
		void setWindowProperties();

		QString encDirPath;
		QString decDirPath;

		QLineEdit *encPathLE;
		QLineEdit *decPathLE;
		NBPasswordEdit *passwd1LE;
		NBPasswordEdit *passwd2LE;

		bool mOkay;

	private Q_SLOTS:
		void validatePassword( const QString& );

	Q_SIGNALS:
		void closed();
};

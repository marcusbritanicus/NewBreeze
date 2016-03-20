/*
	*
	* NBSalsa20.hpp - NewBreeze File Encryption Class Header
	*
*/

#pragma once

#include <Global.hpp>
#include <Salsa20.hpp>
#include <NBPasswordDialog.hpp>
#include <NBPasswordInput.hpp>
#include <NBTools.hpp>

class NBSalsa20 : public QObject {
	Q_OBJECT

	public:
		NBSalsa20( QString path, QWidget* );

	private:
		void makeKeyFromString( QString, uint8_t* );
		void makeIVFromString( QString, uint8_t* );

		QWidget *mParent;
		QString mFilePath;

	public slots:
		void encrypt();
		void decrypt();
		void changePass();

	private slots:
		void validateOldPassword();

	Q_SIGNALS:
		void proceed( bool );
};

/*
	*
	* NBSalsa20.hpp - NewBreeze File Encryption Class Header
	*
*/

#pragma once

#include <QtCore>
#include "./Salsa20.hpp"

class NBSalsa20 : public QObject {
	Q_OBJECT

	public:
		NBSalsa20( QString path = QString(), QWidget *parent = 0 );

		void encrypt( QByteArray password );
		void decrypt( QByteArray password );

		QByteArray encryptData( QByteArray data, QByteArray password );
		QByteArray decryptData( QByteArray data, QByteArray password );

	private:
		void makeKeyFromString( QByteArray, uint8_t* );
		void makeIVFromString( QByteArray, uint8_t* );

		QWidget *mParent;
		QString mFilePath;
};

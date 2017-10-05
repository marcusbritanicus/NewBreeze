/*
	*
	* NBDerypt.hpp - NewBreeze File Folder Decryption Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBTools.hpp"

class NBEncFS : public QObject {
	Q_OBJECT

	public:
		/* Init */
		NBEncFS( QString, QString target = QString(), QWidget *parent = NULL );

	public Q_SLOTS:
		void mountDir( QString );
		void unmountDir();

		void createEncFS( QString, QString, QString );
		void changePass( QString, QString );

	private:
		QString mSource;
		QString mTarget;

		QWidget *mParent;
};

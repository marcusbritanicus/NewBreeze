/*
	*
	* NBDerypt.hpp - NewBreeze Folder Encryption Class Header
	*
*/

#pragma once

#ifdef STANDALONE
	#include "Global2.hpp"
#else
	#include "Global.hpp"
#endif
#include "NBTools.hpp"

class NBEncFS : public QObject {
	Q_OBJECT

	public:
		/* Init */
		NBEncFS( QString, QString target = QString(), QWidget *parent = NULL );

	public Q_SLOTS:
		bool mountDir( QString );
		bool unmountDir();

		bool createEncFS( QString );
		bool changePass( QString, QString );

	private:
		QString mSource;
		QString mTarget;

		QWidget *mParent;
};

class NBCreateEncFS : public QDialog {
	Q_OBJECT

	public:
		/* Init */
		NBCreateEncFS( QWidget *parent = NULL );

		QString directoryName();

	private:
		void createGUI();

		QLineEdit *dirLE;

		QString mSource;
		QString mTarget;

		QPushButton *createBtn;

	private Q_SLOTS:
		void updateButton();
};

/*
	*
	* NBDerypt.hpp - CoreApps Folder Encryption Class Header
	*
*/

#pragma once

#include "Global.hpp"

class CryFS : public QObject {
	Q_OBJECT

	public:
		/* Init */
		CryFS( QString, QString target = QString(), QWidget *parent = NULL );

	public Q_SLOTS:
		bool mountDir( QString );
		bool unmountDir();

		bool createCryFS( QString );

	private:
		QString mSource;
		QString mTarget;

		QWidget *mParent;
};

class CreateCryFS : public QDialog {
	Q_OBJECT

	public:
		/* Init */
		CreateCryFS( QWidget *parent = NULL );

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

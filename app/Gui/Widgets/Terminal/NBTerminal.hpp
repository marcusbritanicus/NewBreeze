/*
    *
    * NBTerminal.hpp - Wordy'NBTerminal.cpp Header
    *
*/

#pragma once

#include "Global.hpp"
#include "qtermwidget.h"

class NBTerminalWidget : public QTermWidget {
	Q_OBJECT

	public :
		NBTerminalWidget( QString wDir, QWidget * );
		~NBTerminalWidget();

		QString currentWorkingDirectory();

	private :
		void setupCwdAutoDetect();

		QString nb3rc, cwdfn, oldcwd;

	private Q_SLOTS:
		void handleCwdChange( QString fn );

	Q_SIGNALS:
		void chdir( QString );
};

class NBTerminal : public QWidget {
	Q_OBJECT

	public :
		NBTerminal( QString wDir = QString(), QWidget *parent = NULL );
		void changeDir( QString );

	private:
		NBTerminalWidget *Terminal;
		QString currentPath;

	public slots:
		void show();
		void hide();

	private slots:
		void openNewTerminal();

	Q_SIGNALS:
		void shown( bool );
		void chdir( QString );
};

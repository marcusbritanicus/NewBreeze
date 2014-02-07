/*
    *
    * NBTerminal.hpp - Wordy'NBTerminal.cpp Header
    *
*/

#pragma once
#ifndef NBTerminal_HPP
#define NBTerminal_HPP

#include <Global.hpp>
#include <qtermwidget.h>

class NBTerminalWidget : public QTermWidget {
	Q_OBJECT

	public :
		NBTerminalWidget( QString wDir );
		QString currentWorkingDirectory();
};

class NBTerminal : public QWidget {
	Q_OBJECT

	public :
		NBTerminal( QString wDir = QString() );
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
};

#endif

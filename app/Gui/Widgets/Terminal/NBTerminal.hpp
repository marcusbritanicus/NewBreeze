/*
	*
	* NBTerminal.hpp - NBTerminal.cpp Header
	*
*/

#pragma once

#include "Global.hpp"
#include "TtyTabWidget.hpp"
#include "NBTermWidget.hpp"

class NBTerminal : public QMainWindow {
	Q_OBJECT

	public :
		NBTerminal( QString wDir = QString(), QString cmd = QString(), QWidget *parent = 0  );
		~NBTerminal();

		TtyTabWidget *TabWidget;

	private :
		void createGUI();
		void startTerminal();
		void setupActions();
		void setWindowProperties();

		QString mWorkDir;
		QString mCmd;

	private slots:
		void showSettings();

		void openFMgr();

	protected :
		void closeEvent( QCloseEvent *cEVent );
};

class NBTerminalWidget : public QWidget {
	Q_OBJECT

	public :
		NBTerminalWidget( QString wDir = QString(), QWidget *parent = NULL );
		void changeDir( QString );

	private:
		NBTermWidget *Terminal;
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

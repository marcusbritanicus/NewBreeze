/*
	*
	* NBTerminal.hpp - NBTerminal.cpp Header
	*
*/

#ifndef QTMAIN_HPP
#define QTMAIN_HPP

#include "Global.hpp"
#include "TtyTabWidget.hpp"

class NBTerminal : public QMainWindow {
	Q_OBJECT

	public :
		NBTerminal( QString workdir, QWidget *parent = 0 );
		NBTerminal( QString workdir, QString cmd, QWidget *parent = 0 );
		~NBTerminal();

	private :
		void createGUI();
		void setupActions();
		void setWindowProperties();

		TtyTabWidget *TabWidget;

	public slots :
		void showHide();

	private slots:
		void showSettings();

		void openFMgr();

	protected :
		void closeEvent( QCloseEvent *cEVent );
};

#endif

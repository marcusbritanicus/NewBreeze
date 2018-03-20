/*
    *
    * TtyTabWidget.hpp - TtyTabWidget.cpp header
    *
*/

#ifndef TTYTABWIDGET_HPP
#define TTYTABWIDGET_HPP

#include "Global.hpp"

class TtyTabWidget : public QTabWidget {
	Q_OBJECT

	public :
		TtyTabWidget( QWidget *parent, bool mode = false );

	private:
		QToolButton *cBtn;
		bool ddMode;

	public slots:
		int newTerminal();
		int newTerminal( QString, QString cmd );
		int newTerminalCWD();
		void clearTerminal();
		void copyToClipboard();
		void pasteClipboard();

		void prevTerminal();
		void nextTerminal();

		void closeTab();

	private slots:
		void closeTab( int );
		void printSelection( bool );

	Q_SIGNALS:
		void close();
};

#endif

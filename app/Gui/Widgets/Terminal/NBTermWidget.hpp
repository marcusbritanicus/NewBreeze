/*
    *
    * NBTermWidget.hpp - NBTermWidget.cpp Header
    *
*/

#pragma once

#include <Global.hpp>
#include <qtermwidget.h>

class NBTermWidget : public QTermWidget {
	Q_OBJECT

	public :
		NBTermWidget( QWidget *parent = 0 );
		NBTermWidget( QString, QWidget *parent = 0 );
		NBTermWidget( QString, QString, QWidget *parent = 0 );

		QString currentWorkingDirectory();
		void changeDir( const QString &dir );

	private:
		void init();

		QString oldCWD;
		QBasicTimer chTimer;

	private slots:
		void reloadSettings();
		void tryChdir();

	protected:
		void timerEvent( QTimerEvent* );

	Q_SIGNALS:
		void chdir( QString );
};

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

	private:
		QString oldCWD;
		QFileSystemWatcher *watcher = new QFileSystemWatcher();

	private slots:
		void reloadSettings();
		void handleFSWSignals( QString );

	Q_SIGNALS:
		void chDir( QString );
};

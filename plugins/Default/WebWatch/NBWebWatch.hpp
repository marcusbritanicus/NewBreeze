/*
	*
	* NBWebWatch.hpp - NewBreeze WebPreviewWidget Class Header
	*
*/

#pragma once

/* Only used for WebEngine */
#include "Global.hpp"
#include <QtWidgets>
#include <QtWebEngine>
#include <QtWebEngineWidgets>

class NBWebWatch : public QDialog {
	Q_OBJECT

	public :
		NBWebWatch( QString path = QDir::homePath() );
		QString path;
		QWebEngineView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

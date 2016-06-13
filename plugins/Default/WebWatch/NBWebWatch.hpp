/*
	*
	* NBWebWatch.hpp - NewBreeze WebPreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include <QtWebKit>

#if QT_VERSION >= 0x050000
	#include <QtWebKitWidgets>
#endif

class NBWebWatch : public QDialog {
	Q_OBJECT

	public :
		NBWebWatch( QString path = QDir::homePath() );
		QString path;
		QWebView *peekWidgetBase;

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

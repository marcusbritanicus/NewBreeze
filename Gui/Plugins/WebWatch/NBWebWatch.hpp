/*
	*
	* NBWebWatch.hpp - NewBreeze WebPreviewWidget Class Header
	*
*/

#ifndef NBWEBWATCH_HPP
#define NBWEBWATCH_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <QtWebKit>

class NBWebWatch : public QWidget {
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
};

#endif

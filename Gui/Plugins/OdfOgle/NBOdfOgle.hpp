/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBODFOGLE_HPP
#define NBODFOGLE_HPP

#include <Global.hpp>
#include <NBTools.hpp>
#include <converter.h>

class NBOdfOgle : public QWidget {
	Q_OBJECT

	public :
		NBOdfOgle( QString path = QDir::homePath() );
		QString path;
		QTextBrowser *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();
		QTextDocument* loadFile();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );
};

#endif

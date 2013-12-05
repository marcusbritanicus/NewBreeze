/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBWORDVIEW_HPP
#define NBWORDVIEW_HPP

#include <Global.hpp>
#include <NBTools.hpp>

class NBWordView : public QWidget {
	Q_OBJECT

	public :
		NBWordView( QWidget *parent = 0, QString path = QDir::homePath() );
		QString path;
		QTextEdit *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();
		QString loadFile();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
};

#endif

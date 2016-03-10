/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBWORDVIEW_HPP
#define NBWORDVIEW_HPP

#include <Global.hpp>

class NBWordView : public QDialog {
	Q_OBJECT

	public :
		NBWordView( QString path = QDir::homePath() );
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
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

#endif

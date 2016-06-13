/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBWordView : public QDialog {
	Q_OBJECT

	public :
		NBWordView( QString path = QDir::homePath() );

	private :
		void createGUI();
		void setWindowProperties();

		QString path;
		QTextEdit *peekWidgetBase;

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadFile();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

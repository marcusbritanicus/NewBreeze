/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "converter.h"

class NBOdfOgle : public QDialog {
	Q_OBJECT

	public :
		NBOdfOgle( QString path = QDir::homePath() );

	private :
		void createGUI();
		void setWindowProperties();

		QString path;
		QTextBrowser *peekWidgetBase;

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

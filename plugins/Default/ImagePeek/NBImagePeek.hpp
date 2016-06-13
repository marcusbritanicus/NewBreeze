/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBImagePeek : public QDialog {
	Q_OBJECT

	public :
		NBImagePeek( QString path = QDir::homePath() );
		QString path;
		QLabel *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadImage();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

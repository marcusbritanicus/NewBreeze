/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBGuiWidgets.hpp"
#include "NBIconManager.hpp"

class NBCustomPeek : public QWidget {
	Q_OBJECT

	public :
		NBCustomPeek( QString path = QDir::homePath(), QWidget *parent = NULL );
		QString path;
		QWidget *peekWidgetBase;
		QLabel *iconLbl, *nameLbl, *pathLbl, *mimeLbl, *sizeLbl, *timeLbl, *ugidLbl;

	private :
		void createGUI();
		void setWindowProperties();
		void getFileProps();

	private slots :
		void openInExternal();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

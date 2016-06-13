/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBIconViewRestricted.hpp"

class NBFolderFlash : public QWidget {
	Q_OBJECT

	public :
		NBFolderFlash( QString path = QDir::homePath(), QWidget *parent = NULL );
		QString path;
		NBIconViewRestricted *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	private slots :
		void loadFolder();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );

	Q_SIGNALS:
		void loadFolder( QString );
};

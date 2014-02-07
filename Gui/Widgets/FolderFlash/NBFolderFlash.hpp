/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once
#ifndef NBFOLDERFLASH_HPP
#define NBFOLDERFLASH_HPP

#include <Global.hpp>
#include <NBIconViewRestricted.hpp>
#include <NBTools.hpp>

class NBFolderFlash : public QWidget {
	Q_OBJECT

	public :
		NBFolderFlash( QString path = QDir::homePath() );
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

	Q_SIGNALS:
		void loadFolder( QString );
};

#endif

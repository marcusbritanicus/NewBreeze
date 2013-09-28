/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBFOLDERFLASH_HPP
#define NBFOLDERFLASH_HPP

#include <Global.hpp>
#include <NBFolderViewRestricted.hpp>

class NBFolderFlash : public QWidget {
	Q_OBJECT

	public :
		NBFolderFlash( QWidget *parent = 0, QString path = QDir::homePath() );
		QString path;
		NBFolderViewRestricted *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	private slots :
		void loadFolder();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
};

#endif

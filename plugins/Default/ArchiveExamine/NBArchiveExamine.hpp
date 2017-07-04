/*
	*
	* NBArchiveExamine.hpp - NewBreeze ArchiveExamine Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBArchiveTreeModel.hpp"

class NBArchiveExamine : public QDialog {
	Q_OBJECT

	public :
		NBArchiveExamine( QString path = QDir::homePath(), QWidget *parent = NULL );
		QString path;
		QTreeView *peekWidgetBase;

	private :
		void createGUI();
		void setWindowProperties();

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadArchive();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

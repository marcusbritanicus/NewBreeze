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

	private :
		void createGUI();
		void setWindowProperties();

		QString path;
		QTreeView *peekWidgetBase;
		NBArchiveTreeModel *mdl;

	public slots:
		int exec();

	private slots :
		void openInExternal();
		void loadArchive();

		void extractSelection();

	protected :
		void keyPressEvent( QKeyEvent *keyEvent );
		void changeEvent( QEvent *cEvent );

		void paintEvent( QPaintEvent *pEvent );
};

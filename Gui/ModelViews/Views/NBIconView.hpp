/*
	*
	* NBIconView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBICONVIEW_HPP
#define NBICONVIEW_HPP

#include <Global.hpp>
#include <NBIconDelegate.hpp>
#include <NBContextMenu.hpp>
#include <NBFileSystemModel.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>

class NBIconView : public QListView {
	Q_OBJECT

	public :
		NBIconView( NBFileSystemModel *fsm );

		void updateViewMode();
		void setContentsWidth( int );

		NBFileSystemModel *fsModel;
		NBIconDelegate *iDelegate;

		NBCustomActionsMenu *customMenu;

	private :
		void createAndSetupActions();
		QSize getGridSize( QSize );

		int contentsWidth;
		int itemsPerRow;
		int numberOfRows;

		QPoint dragStartPosition;

	Q_SIGNALS :
		void open( QModelIndex );
		void contextMenuRequested( QPoint );
		void copy( QStringList, QString );
		void move( QStringList, QString );
		void link( QStringList, QString );

	protected:
		void mousePressEvent( QMouseEvent * );
		void mouseMoveEvent( QMouseEvent * );

		void mouseDoubleClickEvent( QMouseEvent * );
		void keyPressEvent( QKeyEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );
};

#endif

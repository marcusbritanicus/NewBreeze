/*
	*
	* NBTreeView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBTreeView_HPP
#define NBTreeView_HPP

#include <Global.hpp>
#include <NBTreeDelegate.hpp>
#include <NBContextMenu.hpp>
#include <NBFileSystemModel.hpp>
#include <NBTools.hpp>
#include <NBIconProvider.hpp>

class NBTreeView : public QTreeView {
	Q_OBJECT

	public :
		NBTreeView( NBFileSystemModel * );

		void updateViewMode();

		NBFileSystemModel *fsModel;
		// NBTreeDelegate *iDelegate;

		NBCustomActionsMenu *customMenu;

	private :
		void createAndSetupActions();
		QSize getGridSize( QSize );

	Q_SIGNALS :
		void open( QModelIndex );
		void newWindow( QString );
		void contextMenuRequested( const QPoint &position );
		void copy( QStringList, QString );
		void move( QStringList, QString );
		void link( QStringList, QString );

	protected:
		void mouseDoubleClickEvent( QMouseEvent * );

		void dragEnterEvent( QDragEnterEvent* );
		void dragMoveEvent( QDragMoveEvent* );
		void dropEvent( QDropEvent* );
};

class ThreadedDelete : public QThread {

	public :
		void run();

		QStringList delPaths;
};

#endif

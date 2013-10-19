/*
	*
	* NBFDFolderView.hpp - NBFDFolderView.cpp Header
	*
*/

#ifndef NBFDFolderView_HPP
#define NBFDFolderView_HPP

#include <Global.hpp>
#include <NBFileSystemModel.hpp>
#include <NBFDItemDelegate.hpp>
#include <NBContextMenu.hpp>

class NBFDFolderView : public QListView {
	Q_OBJECT

	public :
		NBFDFolderView( QString location );
		QModelIndexList getSelection();

		void updateViewMode();
		void setContentsWidth( int );

		QAction *reload;
		QAction *actPrevDir, *actNextDir, *actParDir, *showHideDotFiles;
		QAction *actNewDir, *actNewFile;
		QAction *sortByNameAct, *sortByTypeAct, *sortBySizeAct, *sortByDateAct;

		NBFileSystemModel *fsModel;
		NBFDItemDelegate *iDelegate;

		bool showHidden;
		NBCustomActionsMenu *customMenu;

	private :
		void createAndSetupActions();
		QSize getGridSize( QSize );

		QFileSystemWatcher *watchDog;
		QString copySrc;
		bool toBeCut;

		int contentsWidth;
		bool __modelSet;

	public slots :
		void goUp();
		void goBack();
		void goForward();
		void goHome();

		void newFolder();
		void newFile();

		void doOpen( QModelIndex );
		void doOpen( QString loc );
		void doReload();
		void doShowHideDotFiles();

		void sortByName();
		void sortByType();
		void sortBySize();
		void sortByDate();

		void toBeImplemented();
		void contextMenuRequested( const QPoint &position );

	private slots :
		void handleWatchDogBark( QString path );

	Q_SIGNALS :
		void updateToolBar();
		void updatePropsView( QStringList );
		void showHideSideBar();

	protected:
		void mouseDoubleClickEvent( QMouseEvent * );
};

#endif

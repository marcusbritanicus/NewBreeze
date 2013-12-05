/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#ifndef NBFOLDERVIEW_HPP
#define NBFOLDERVIEW_HPP

#include <Global.hpp>
#include <NBIconView.hpp>
#include <NBTreeView.hpp>
#include <NBContextMenu.hpp>
#include <NBFileSystemModel.hpp>
#include <NBFVDialogs.hpp>
#include <NBFileDialog.hpp>
#include <NBFolderFlash.hpp>
#include <NBWordView.hpp>
#include <NBWebWatch.hpp>
#include <NBImagePeek.hpp>
#include <NBCustomPeek.hpp>
#include <NBConfirmDeleteDialog.hpp>
#include <NBPropertiesDialog.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBArchive.hpp>
#include <NBCustomActions.hpp>
#include <NBMessageDialog.hpp>
#include <NBDeleteManager.hpp>
#include <NBIconProvider.hpp>
#include <NBPasteDialog.hpp>

class NBFolderView : public QStackedWidget {
	Q_OBJECT

	public :
		NBFolderView();

		void updateViewMode();

		QModelIndexList getSelection();
		bool hasSelection();

		QAction *peekAct, *moveAct, *copyAct, *pasteAct, *renameAct, *reloadAct, *trashAct, *delAct, *propertiesAct, *permissionsAct;
		QAction *actPrevDir, *actNextDir, *actParDir, *actHomeDir, *showHideDotFiles, *openVTE;
		QAction *actNewDir, *actNewFile;
		QAction *sortByNameAct, *sortByTypeAct, *sortBySizeAct, *sortByDateAct;
		QAction *groupsAct;

		NBIconView *IconView;
		NBTreeView *TreeView;

		NBFileSystemModel *fsModel;
		NBCustomActionsMenu *customMenu;

	private :
		QModelIndex indexAt( QPoint );
		void createAndSetupActions();

		QClipboard *ClipBoard = qApp->clipboard();
		bool moveItems;

	public slots :
		void goUp();
		void goBack();
		void goForward();

		void newFolder();
		void newFile();

		void doOpen( QModelIndex );
		void doOpen( QString loc );
		void doOpenHome();
		void doOpenWith();
		void doOpenInNewWindow();
		void doOpenWithCmd();

		void doRename();
		void doPeek();
		void doReload();
		void doToggleHidden();
		void move( QStringList, QString );
		void acopy( QStringList, QString );
		void copy( QStringList, QString );
		void link( QStringList, QString );
		void doSendToTrash();
		void doDelete();

		void selectAll();

		void sortByName();
		void sortByType();
		void sortBySize();
		void sortByDate();

		void openTerminal();
		void showContextMenu( QPoint );

		// Over-riding the setFocus() of QWidget
		void setFocus();

	private slots :
		void prepareCopy();
		void prepareMove();
		void prepareIO();

		void handleWatchDogBark( QString path );
		void extract( QString );
		void compress( QStringList );

		void updateModel( QString );
		void updateProgress( QString, float, float );
		void handleDeleteFailure( QStringList, QStringList );

	Q_SIGNALS :
		void newWindow( QString );
		void selectionChanged( const QItemSelection&, const QItemSelection& );

		void move( QStringList, QString, NBIOMode::Mode );
		void copy( QStringList, QString, NBIOMode::Mode );

		void focusSearchBar();
		void clearSearchBar();

		void showProperties();
		void showPermissions();

		void reloadCatalogs();
		void toggleGroups();
};

#endif

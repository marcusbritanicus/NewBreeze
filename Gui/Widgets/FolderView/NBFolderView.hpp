/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#pragma once
#ifndef NBFOLDERVIEW_HPP
#define NBFOLDERVIEW_HPP

#include <Global.hpp>
#include <NBIconView.hpp>
#include <NBContextMenu.hpp>
#include <NBFileSystemModel.hpp>
#include <NBFVDialogs.hpp>
#include <NBFileDialog.hpp>
#include <NBConfirmDeleteDialog.hpp>
#include <NBPropertiesDialog.hpp>
#include <NBTools.hpp>
#include <NBGuiWidgets.hpp>
#include <NBArchive.hpp>
#include <NBCustomActions.hpp>
#include <NBMessageDialog.hpp>
#include <NBDeleteManager.hpp>
#include <NBIconProvider.hpp>
#include <NBAppsView.hpp>
#include <NBCatalogView.hpp>
#include <NBStyleManager.hpp>
#include <NBDefaultPlugins.hpp>
#include <NBCustomPeek.hpp>
#include <NBFolderFlash.hpp>

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
		NBApplicationsView *ApplicationsView;
		NBCatalogView *CatalogView;

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
		void doOpenInNewTab();
		void doOpenWithCmd();

		void showApplications();
		void showCatalogs();
		void showFolders();

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
		void newTab( QString );
		void selectionChanged( const QItemSelection&, const QItemSelection& );

		void move( QStringList, QString, NBIOMode::Mode );
		void copy( QStringList, QString, NBIOMode::Mode );

		void focusSearchBar();
		void clearSearchBar();

		void showProperties();
		void showPermissions();

		void reloadCatalogs();
		void toggleGroups();

		void updateAddressBar( QString );
};

#endif

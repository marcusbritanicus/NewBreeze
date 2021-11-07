/*
	*
	* NBFolderView.hpp - NewBreeze FolderView Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBArchiveDialog.hpp"
#include "NBConfirmDeleteDialog.hpp"
#include "NBContextMenu.hpp"
#include "NBCustomActions.hpp"
#include "NBDeleteManager.hpp"
#include "NBDeleteProcess.hpp"
#include "NBErrorsDialog.hpp"
#include "NBEncFS.hpp"
#include "NBFileDialog.hpp"
#include "NBFunctions.hpp"
#include "NBItemViewModel.hpp"
#include "NBGuiWidgets.hpp"
#include "NBIconManager.hpp"
#include "NBIconView.hpp"
#include "NBIOProcess.hpp"
#include "NBLogger.hpp"
#include "NBMessageDialog.hpp"
#include "NBNewNodeDialog.hpp"
#include "NBPluginManager.hpp"
#include "NBProcessManager.hpp"
#include "NBPropertiesDialog.hpp"
#include "NBRenameDialog.hpp"
#include "NBRunCmdDialog.hpp"
#include "NBTerminal.hpp"
#include "NBVault.hpp"
#include "NBVaultDatabase.hpp"

class NBFolderView : public QWidget {
	Q_OBJECT

	public :
		NBFolderView( QWidget *parent = NULL);

		void updateViewMode();

		QModelIndexList getSelection();
		bool hasSelection();

		QAction *peekAct, *moveAct, *copyAct, *pasteAct, *renameAct, *reloadAct, *trashAct, *delAct, *propertiesAct, *permissionsAct;
		QAction *actPrevDir, *actNextDir, *actParDir, *actHomeDir, *actGoHome, *showHideDotFiles, *openVTE, *openVTEin;
		QAction *actNewDir, *actNewFile, *actNewEncFS;
		QAction *sortByNameAct, *sortByTypeAct, *sortBySizeAct, *sortByDateAct;
		QAction *groupsAct;
		QAction *addBookMarkAct, *addToSuperStartAct;

		NBIconView *IconView;

		NBItemViewModel *fsModel;

	private :
		QModelIndex indexAt( QPoint );
		void createAndSetupActions();

		static bool moveItems;

		QClipboard *clipBoard;
		NBProcessManager *pMgr;

		NBActionsMenu *customMenu;

	public slots :
		void goUp();
		void goBack();
		void goForward();

		void newFolder();
		void newFile();

		void doOpen( QModelIndex );
		void doOpen( QString loc );
		void doOpenHome();
		void doOpenSS();
		void loadHomeDir();
		void doOpenWith();
		void doOpenInNewWindow();
		void doOpenInNewTab();
		void doOpenWithCmd();

		void showFolders();

		void doRename();
		void doPeek( QModelIndex curIdx = QModelIndex() );
		void doReload();
		void doToggleHidden();
		void move( QStringList, QString );
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
		void openTerminalIn();
		void showContextMenu( QPoint );
		void showActionsMenu( QPoint );

		// Over-riding the setFocus() of QWidget
		void setFocus();

	private slots :
		void prepareCopy();
		void prepareMove();
		void prepareIO();

		void handleWatchDogBark( QString path );
		void extract( QString );
		void compress( QStringList );

		void createEncFS();

		void updateActions();
		void addBookMark();
		void addToSuperStart();

		void archiveJob() {

			QMessageBox::information(
				this,
				"NewBreeze | NBArchive",
				QString(
					"NewBreeze encountered some issue while working with the archive. "
					"If you are consistently facing issues with creation and extraction of archives, "
					"please open a bug report by going to <b>NewBreeze Menu</b> and selecting <b>Report Bug</b>"
				)
			);
		};

	Q_SIGNALS :
		void newWindow( QString );
		void newTab( QString );
		void selectionChanged( const QItemSelection&, const QItemSelection& );

		void archiveDone( QString, QChar );

		void showProperties();
		void showPermissions();

		void reloadCatalogs();
		void reloadBookmarks();
		void reloadSuperStart();

		void toggleGroups();

		void hideStatusBar();
		void showStatusBar();
};

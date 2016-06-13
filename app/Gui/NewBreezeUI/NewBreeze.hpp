/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#pragma once

#include "Global.hpp"

#include "AboutNB.hpp"
#include "NBAddressBar.hpp"
#include "NBCustomActions.hpp"
#include "NBFilterWidget.hpp"
#include "NBFolderView.hpp"
#include "NBGuiWidgets.hpp"
#include "NBInfoBar.hpp"
#include "NBLogger.hpp"
#include "NBMessageDialog.hpp"
#include "NBPropertiesDialog.hpp"
#include "NBSettingsManager.hpp"
#include "NBSideBar.hpp"
#include "NBSidePanel.hpp"
#include "NBTerminal.hpp"
#include "NBTrashManager.hpp"

class NewBreeze : public QMainWindow {
	Q_OBJECT

	public :
		NewBreeze( QString loc = QString() );

	private :
		/* Create our UI */
		void createGUI();

		/* Setup the SidePanel */
		void setupSidePanel();

		/* Set window title, icon, size etc */
		void setWindowProperties();

		/* Create and Setup signal/slot mechanisms */
		void createAndSetupActions();

		/* Open a file */
		void openFile( QString );

		NBAddressBar *AddressBar;
		NBSideBar *SideBar;
		NBSidePanel *SidePanel;
		NBFolderView *FolderView;
		NBTerminal *Terminal;
		NBInfoBar *InfoBar;
		NBFilterWidget *FilterWidget;

		bool mTerminate;

	public Q_SLOTS:
		void show();

	private Q_SLOTS:
		void showAboutNB();
		void showAboutQt();
		void showLicense();

		void showInfoDlg();
		void showSettingsDialog();
		void showCustomActionsDialog();

		void newWindow( QString path = QString() );

		void handleMessages( const QString );

		void updateInfoBar();
		void updateVarious( QString url = QString() );

		void showProperties();
		void showPermissions();

		void handleDriveUrl( QString );
		void showApplications();
		void showCatalogs();
		void showFolders();
		void showTrash();

		void filterFiles( QString );
		void clearFilters();

		void initiateIO( QStringList, QString, NBIOMode::Mode );

		void addBookMark();

		void openAddressBar();
		void openWithList();

		void switchToNextView();
		void changeViewMode( int );
		void toggleGrouping();

		void toggleSideBarVisible();
		void showHideTermWidget();

		/* Busy cursor during loading of models */
		void setBusyCursor();
		void setNormalCursor();

		void quit();

	protected:
		void closeEvent( QCloseEvent *cEvent );
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

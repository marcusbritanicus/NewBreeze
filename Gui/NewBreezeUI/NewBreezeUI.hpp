/*
	*
	* NewBreezeUI.hpp - NewBreeze UI Header
	*
*/

#pragma once

#include <Global.hpp>

#include <NBSidePanel.hpp>
#include <NBFolderView.hpp>
#include <NBAddressBar.hpp>
#include <NBInfoBar.hpp>
#include <NBPropertiesDialog.hpp>
#include <NBGuiWidgets.hpp>
#include <NBTerminal.hpp>
#include <NBIOManager.hpp>
#include <NBTools.hpp>
#include <NBCustomActions.hpp>
#include <NBSettingsManager.hpp>
#include <NBMessageDialog.hpp>
#include <NBTrashManager.hpp>
#include <NBUtilityBar.hpp>
#include <NBFilterWidget.hpp>

class NewBreezeUI : public QWidget {
	Q_OBJECT

	public :
		NewBreezeUI( QString loc );

		void setFocus();
		bool activeJobs();
		void showActiveJobs();

		NBUtilityBar *uBar;
		NBAddressBar *AddressBar;
		NBSidePanel *SidePanel;
		NBFolderView *FolderView;
		NBTerminal *Terminal;
		NBInfoBar *InfoBar;
		NBFilterWidget *FilterWidget;

	private :
		void createGUI();
		void createAndSetupActions();

		QAction *addBookMarkAct;

		// Kill switch for Properties
		bool __terminate;

	private slots:
		void addBookMark();

		void openAddressBar();
		void openAddress( QString );
		void openWithList();
		void openNewWindow();

		void clearSearch();

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

		void updateInfoBar();
		void updateVarious( QString );

		void switchToNextView();
		void changeViewMode( int );
		void toggleGrouping();

		void toggleSideBarVisible();
		void showHideTermWidget();

	Q_SIGNALS:
		void newWindow( QString );
		void restoreWindow();
		void maximizeWindow();
		void minimizeWindow();

		void closeWindow();
		void titleBarMousePress( QMouseEvent * );
		void titleBarMouseMove( QMouseEvent * );
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

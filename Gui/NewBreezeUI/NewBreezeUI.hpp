/*
	*
	* NewBreezeUI.hpp - NewBreeze UI Header
	*
*/

<<<<<<< HEAD
#pragma once
=======
#ifndef NEWBREEZEUI_HPP
#define NEWBREEZEUI_HPP
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

#include <Global.hpp>

#include <NBSidePanel.hpp>
#include <NBFolderView.hpp>
#include <NBAddressBar.hpp>
<<<<<<< HEAD
=======
#include <NBQuickMenuBar.hpp>
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
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

class NewBreezeUI : public QWidget {
	Q_OBJECT

	public :
		NewBreezeUI( QString loc );
<<<<<<< HEAD

		void setFocus();
		bool activeJobs();
		void showActiveJobs();

		NBUtilityBar *uBar;
		NBAddressBar *AddressBar;
=======
		void updateGUI();

		void setFocus();

		NBAddressBar *AddressBar;
		NBQuickMenuBar *QuickMenuBar;
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
		NBSidePanel *SidePanel;
		NBFolderView *FolderView;
		NBTerminal *Terminal;
		NBInfoBar *InfoBar;

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

<<<<<<< HEAD
=======
		void focusSearch();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
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
<<<<<<< HEAD
=======
		void updateQuickMenuBar();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

		void switchToNextView();
		void changeViewMode();
		void toggleGrouping();

		void toggleSideBarVisible();
		void showHideTermWidget();

	Q_SIGNALS:
<<<<<<< HEAD
		void newWindow( QString );
		void restoreWindow();
		void maximizeWindow();
		void minimizeWindow();

		void closeWindow();
		void titleBarMousePress( QMouseEvent * );
		void titleBarMouseMove( QMouseEvent * );
=======
		void addJob( QStringList, QString, NBIOMode::Mode );
		void newWindow( QString );
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();
<<<<<<< HEAD
=======

#endif
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

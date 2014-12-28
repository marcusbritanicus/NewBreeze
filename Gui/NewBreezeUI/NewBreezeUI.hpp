/*
	*
	* NewBreezeUI.hpp - NewBreeze UI Header
	*
*/

#ifndef NEWBREEZEUI_HPP
#define NEWBREEZEUI_HPP

#include <Global.hpp>

#include <NBSidePanel.hpp>
#include <NBFolderView.hpp>
#include <NBAddressBar.hpp>
#include <NBQuickMenuBar.hpp>
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
		void updateGUI();

		void setFocus();

		NBAddressBar *AddressBar;
		NBQuickMenuBar *QuickMenuBar;
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

		void focusSearch();
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
		void updateQuickMenuBar();

		void switchToNextView();
		void changeViewMode();
		void toggleGrouping();

		void toggleSideBarVisible();
		void showHideTermWidget();

	Q_SIGNALS:
		void addJob( QStringList, QString, NBIOMode::Mode );
		void newWindow( QString );
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif

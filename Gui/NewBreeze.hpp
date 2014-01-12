/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#ifndef NEWBREEZE_HPP
#define NEWBREEZE_HPP

#include <Global.hpp>

#include <NBTitleBar.hpp>
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
#include <NBStyleManager.hpp>

class NewBreeze : public QMainWindow {
	Q_OBJECT

	public :
		enum Views {
			Folders = 0,
			Applications,
			Catalogs
		};

		NewBreeze( QString loc = QString() );

		QLabel *statusName;
		QLabel *statusSize;
		QLabel *statusDate;

		void setWindowTitle( QString );
		void setWindowIcon( QIcon );

	private :
		void createGUI();
		void setWindowProperties();
		void createAndSetupActions();

		void updateGUI();

		NBTitleBar *TitleBar;
		NBAddressBar *AddressBar;
		NBQuickMenuBar *QuickMenuBar;
		NBSidePanel *SidePanel;
		NBFolderView *FolderView;
		NBTerminal *Terminal;
		NBInfoBar *InfoBar;

		QAction *addBookMarkAct;

		QPoint dragPosition;

	public slots:
		void newWindow( QString path = QString() );

	private slots:
		void showAboutNB();
		void showAboutQt4();
		void showInfoDlg();
		void showSettingsDialog();
		void showCustomActionsDialog();

		void addBookMark();

		void openAddressBar();
		void openAddress( QString );
		void openWithList();
		void openNewWindow();
		void openInNewWindow( QString );

		void focusSearch();
		void clearSearch();

		void showProperties();
		void showPermissions();

		void handleDriveUrl( QString );
		void showApplications();
		void showCatalogs();
		void showFolders();

		void filterFiles( QString );
		void clearFilters();

		void initiateIO( QStringList, QString, NBIOMode::Mode );

		void updateInfoBar();
		void updateVarious( QString );
		void updateQuickMenuBar();

		void switchToNextView();
		void changeViewMode();
		void toggleGrouping();

		void toggleMaximizeRestore();
		void toggleSideBarVisible();
		void showHideTermWidget();

		void windowPressStart( QMouseEvent * );
		void windowMoveStart( QMouseEvent * );

	protected:
		void closeEvent( QCloseEvent *cEvent );
		void resizeEvent( QResizeEvent *rEvent );
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif

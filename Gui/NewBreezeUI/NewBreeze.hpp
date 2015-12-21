/*
	*
	* NewBreeze.hpp - NewBreeze Header
	*
*/

#ifndef NEWBREEZE_HPP
#define NEWBREEZE_HPP

#include <Global.hpp>

#include <NewBreezeUI.hpp>
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
<<<<<<< HEAD
#include <NewBreezeTray.hpp>
#include <AboutNB.hpp>
=======
#include <NBUtilityBar.hpp>
#include <NewBreezeTray.hpp>
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223

class NewBreeze : public QMainWindow {
	Q_OBJECT

	public :
		NewBreeze( QString loc = QString(), bool tray = false );

<<<<<<< HEAD
		bool canOpenUI();

		void setWindowTitle( QString );
		void setWindowIcon( QIcon );

		bool showTrayIcon();
		void setShowTrayIcon( bool );
		void showTrayIconInfo( QString );

=======
		void setWindowTitle( QString );
		void setWindowIcon( QIcon );

>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	private :
		void createGUI();
		void setWindowProperties();
		void createAndSetupActions();

<<<<<<< HEAD
		void openFile( QString );

=======
		NBUtilityBar *uBar;
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
		NewBreezeUI *UI;

		QString initDir;
		QPoint dragPosition;

<<<<<<< HEAD
		// No UI Flag for file open
		bool mHaveUI;

=======
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
		// Kill switch for Properties
		bool __terminate;
		int __currentIndex;

<<<<<<< HEAD
		bool mShowTrayIcon;

=======
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
	public slots:
		void newWindow( QString path = QString() );

	private slots:
		void showAboutNB();
		void showAboutQt4();
		void showInfoDlg();
		void showSettingsDialog();
		void showCustomActionsDialog();

<<<<<<< HEAD
=======
		void initiateIO( QStringList, QString, NBIOMode::Mode );

>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
		void toggleMaximizeRestore();
		void windowPressStart( QMouseEvent * );
		void windowMoveStart( QMouseEvent * );

<<<<<<< HEAD
		void handleMessages( const QString );

	protected:
		void closeEvent( QCloseEvent *cEvent );
		void paintEvent( QPaintEvent *pEvent );
=======
	protected:
		void closeEvent( QCloseEvent *cEvent );

	Q_SIGNALS:
		void showTrayIcon();
>>>>>>> 6502fc82ae37a0c63e10954c84d1c0a501a1a223
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif

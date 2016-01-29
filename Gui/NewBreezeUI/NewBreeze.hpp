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
#include <NewBreezeTray.hpp>
#include <AboutNB.hpp>

class NewBreeze : public QMainWindow {
	Q_OBJECT

	public :
		NewBreeze( QString loc = QString(), bool tray = false );

		bool canOpenUI();

		void setWindowTitle( QString );
		void setWindowIcon( QIcon );

		bool showTrayIcon();
		void setShowTrayIcon( bool );
		void showTrayIconInfo( QString );

	private :
		void createGUI();
		void setWindowProperties();
		void createAndSetupActions();

		void openFile( QString );

		NewBreezeUI *UI;

		QString initDir;
		QPoint dragPosition;

		// No UI Flag for file open
		bool mHaveUI;

		// Kill switch for Properties
		bool __terminate;
		int __currentIndex;

		bool mShowTrayIcon;

	public slots:
		void newWindow( QString path = QString() );

	private slots:
		void showAboutNB();
		void showAboutQt4();
		void showInfoDlg();
		void showSettingsDialog();
		void showCustomActionsDialog();

		void toggleMaximizeRestore();
		void windowPressStart( QMouseEvent * );
		void windowMoveStart( QMouseEvent * );

		void handleMessages( const QString );

	protected:
		void closeEvent( QCloseEvent *cEvent );
		void paintEvent( QPaintEvent *pEvent );
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif

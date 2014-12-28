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
#include <NewBreezeTray.hpp>

class NewBreeze : public QMainWindow {
	Q_OBJECT

	public :
		NewBreeze( QString loc = QString(), bool tray = false );

		void setWindowTitle( QString );
		void setWindowIcon( QIcon );

	private :
		void createGUI();
		void setWindowProperties();
		void createAndSetupActions();

		NBUtilityBar *uBar;
		NewBreezeUI *UI;

		QString initDir;
		QPoint dragPosition;

		// Kill switch for Properties
		bool __terminate;
		int __currentIndex;

	public slots:
		void newWindow( QString path = QString() );

	private slots:
		void showAboutNB();
		void showAboutQt4();
		void showInfoDlg();
		void showSettingsDialog();
		void showCustomActionsDialog();

		void initiateIO( QStringList, QString, NBIOMode::Mode );

		void toggleMaximizeRestore();
		void windowPressStart( QMouseEvent * );
		void windowMoveStart( QMouseEvent * );

	protected:
		void closeEvent( QCloseEvent *cEvent );

	Q_SIGNALS:
		void showTrayIcon();
};

// The code for @f NBStartup() and ReadSettings() is in NBStartup.cpp
void NBStartup();

#endif

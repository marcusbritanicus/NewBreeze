/*
	*
	* NewBreezeTray.cpp - NewBreeze Tray Icon Class
	*
*/

#include <NewBreezeTray.hpp>

NBTrayIcon::NBTrayIcon() : QSystemTrayIcon() {

	setIcon( QIcon( ":/icons/newbreeze2.png" ) );
	connect( this, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( handleActivation( QSystemTrayIcon::ActivationReason ) ) );
};

void NBTrayIcon::handleActivation( QSystemTrayIcon::ActivationReason reason ) {

	switch( reason ) {
		case NBTrayIcon::Context: {
			qApp->quit();
			break;
		};

		case NBTrayIcon::MiddleClick: {
			qApp->quit();
			break;
		};

		case NBTrayIcon::DoubleClick: {
			emit newWindow();
			break;
		};

		default:{
			break;
		};
	};
};

void NBTrayIcon::showInfo() {

	showMessage(
		"TrayIcon initialized",
		"NewBreeze is now open in SystemTray. Double click to open a new window, right click to quit.",
		QSystemTrayIcon::Information,
		2500
	);
};

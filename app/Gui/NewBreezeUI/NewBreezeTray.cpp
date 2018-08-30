/*
	*
	* NewBreezeTray.cpp - NewBreeze Tray Icon Class
	*
*/

#include "NewBreezeTray.hpp"

NBTrayIcon::NBTrayIcon() : QSystemTrayIcon() {

	setIcon( QIcon( ":/icons/newbreeze.png" ) );
	connect( this, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( handleActivation( QSystemTrayIcon::ActivationReason ) ) );

	NBProcessManager *pMgr = NBProcessManager::instance();
	connect( pMgr, SIGNAL( processCompleted( NBProcess::Progress* ) ), this, SLOT( processCompleted( NBProcess::Progress* ) ) );

	QMenu *menu = new QMenu( "TrayMenu" );
	menu->addAction( QIcon( ":/icons/newbreeze.png" ), "New &Window", this, SIGNAL( newWindow() ) );
	menu->addAction( "&Toggle Visible Windows", this, SLOT( toggleVisible() ) );
	menu->addAction( QIcon( ":/icons/delete.png" ), "&Quit NewBreeze", this, SLOT( quit() ) );
	setContextMenu( menu );
};

void NBTrayIcon::handleActivation( QSystemTrayIcon::ActivationReason reason ) {

	switch( reason ) {
		case NBTrayIcon::Context: {
			break;
		};

		case NBTrayIcon::MiddleClick: {
			qApp->quit();
			break;
		};

		case NBTrayIcon::Trigger: {
			toggleVisible();
			break;
		}

		case NBTrayIcon::DoubleClick: {
			emit newWindow();
			break;
		};

		default:{
			break;
		};
	};
};

void NBTrayIcon::processCompleted( NBProcess::Progress *proc ) {

	switch( proc->type ) {
		case NBProcess::Copy: {
			showMessage(
				"Copying finished",
				QString( "Copying of selected nodes from %1 to %2 complete." ).arg( proc->sourceDir ).arg( proc->targetDir ),
				QIcon( ":/icons/copy.png" ),
				5000
			);
			break;
		}

		case NBProcess::Move: {
			showMessage(
				"Moving finished",
				QString( "Moving of selected nodes from %1 to %2 complete." ).arg( proc->sourceDir ).arg( proc->targetDir ),
				QIcon( ":/icons/move.png" ),
				5000
			);
			break;
		}

		case NBProcess::Delete: {
			showMessage(
				"Deleting finished",
				QString( "Deleting of selected nodes from %1 complete." ).arg( proc->sourceDir ),
				QIcon( ":/icons/trash.png" ),
				5000
			);
			break;
		}

		case NBProcess::Trash: {
			showMessage(
				"Deleting finished",
				QString( "Nodes selected in %1 have been sent to trash." ).arg( proc->sourceDir ).arg( proc->targetDir ),
				QIcon( ":/icons/trash.png" ),
				5000
			);
			break;
		}

		case NBProcess::Properties: {
			//showMessage(
				//"Changing properties finished",
				//QString( "Copying of selected nodes from %1." ).arg( proc->sourceDir ).arg( proc->targetDir ),
				//2000
			//);
			break;
		}
	}
};

void NBTrayIcon::toggleVisible() {

	bool visible = true;
	Q_FOREACH( QWidget *nb, qApp->topLevelWidgets() ) {
		if ( qobject_cast<NewBreeze*>( nb ) ) {
			if ( nb->isVisible() )
				visible &= true;

			else
				visible &= false;
		}
	}

	/* All NewBreeze instances are visible; hide them */
	if ( visible ) {
		Q_FOREACH( QWidget *nb, qApp->topLevelWidgets() )
			if ( qobject_cast<NewBreeze*>( nb ) )
				nb->hide();
	}

	/* Some are hidden, show them all */
	else {
		Q_FOREACH( QWidget *w, qApp->topLevelWidgets() ) {
			NewBreeze *nb = qobject_cast<NewBreeze*>( w );
			if ( nb and not nb->isClosed() )
				nb->show();
		}
	}
};

void NBTrayIcon::showInfo() {

	showMessage(
		"TrayIcon initialized",
		"NewBreeze is now open in SystemTray. Double click to open a new window, right click to quit.",
		QSystemTrayIcon::Information,
		2500
	);
};

void NBTrayIcon::quit() {

	/* Close all top level widgets */
	Q_FOREACH( QWidget *w, qApp->topLevelWidgets() )
		w->close();

	fclose( nblog );

	qApp->quit();
};

/*
	*
	* NBProcessManager.cpp - NewBreeze Core Process Class for I/O, permission settings, etc
	*
*/

#include <NBProcessManager.hpp>

NBProcessManager::NBProcessManager( QObject *parent ) : QObject( parent ) {

	procGUI = new NBProcessManagerGUI( this );
	procMiniGUI = new NBProcessManagerGUIMini( this );
};

void NBProcessManager::timerEvent( QTimerEvent *tEvent ) {

	if ( tEvent->timerId() == updateTimer.timerId() ) {
		// Update the minibar and update the main gui when it is opened
		procGUI->
	}

	tEvent->accept();
};

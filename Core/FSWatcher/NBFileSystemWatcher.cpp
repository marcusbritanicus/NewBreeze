/*
	*
	* NBFileSystemWatcher.cpp - INotify Class for Qt4
	*
*/

#include <NBFileSystemWatcher.hpp>

NBFileSystemWatcher::NBFileSystemWatcher() : QThread() {

	inotifyFD = inotify_init();
	if ( inotifyFD < 0 )
		qCritical() << "Failed initialize inotify";
};

NBFileSystemWatcher::~NBFileSystemWatcher() {

	__stopWatch = true;

	inotify_rm_watch( inotifyFD, WD );
	close( inotifyFD );
};

void NBFileSystemWatcher::setWatchPath( QString wPath ) {

	// Set @v watchPath
	watchPath = QString( wPath );

	// Save the old Watch Descriptor
	int oldWD = WD;

	// Add anew watch
	WD = inotify_add_watch( inotifyFD, qPrintable( watchPath ), IN_ALL_EVENTS );
	if ( WD == -1 ) {
		qCritical() << "Couldn't add watch: " << qPrintable( watchPath );
		emit watchFailed();
	}

	// Now delete the old watch
	if ( oldWD >= 0 )
		inotify_rm_watch( inotifyFD, oldWD );
};

void NBFileSystemWatcher::startWatch() {

	__stopWatch = false;
	start();
};

void NBFileSystemWatcher::stopWatch() {

	__stopWatch = true;
};

void NBFileSystemWatcher::run() {

	while ( not __stopWatch ) {
		int length = 0, i = 0;
		while( true ) {
			i = 0;
			length = read( inotifyFD, buffer, BUF_LEN );

			// If for some reason length < 0, continue with trying to read.
			if ( length < 0 )
				continue;

			if ( __stopWatch )
				return;

			while ( i < length ) {
				if ( __stopWatch )
					return;

				struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
				// If this is a transition between two watches, ignore the old watch events
				if ( event->wd != WD )
					break;

				if ( event->mask & IN_DELETE_SELF ) {
					emit watchPathDeleted();
				}

				if ( ( event->mask & IN_CREATE ) ) {
					emit nodeCreated( watchPath + "/" + event->name );
				}

				if ( event->mask & IN_MODIFY ) {
					emit nodeChanged( watchPath + "/" + event->name );
				}

				if ( ( event->mask & IN_DELETE ) ) {
					emit nodeDeleted( watchPath + "/" + event->name );
				}

				i += EVENT_SIZE + event->len;
			}
		}
	}
};

/*
	*
	* NBFileSystemWatcher.cpp - INotify Class for Qt4
	*
*/

#include <NBFileSystemWatcher.hpp>

NBFileSystemWatcher::NBFileSystemWatcher() : QThread() {

	__quitQNotify = false;
	__watchChanging = false;
	__watching = false;

	inotifyFD = inotify_init();
	if ( inotifyFD < 0 )
		qCritical() << "Failed initialize inotify";
};

NBFileSystemWatcher::~NBFileSystemWatcher() {

	__quitQNotify = true;

	inotify_rm_watch( inotifyFD, WD );
	close( inotifyFD );
};

void NBFileSystemWatcher::setWatchPath( QString wPath ) {

	watchPath = QString( wPath );
	__watchChanging = true;
};

void NBFileSystemWatcher::startWatch() {

	__quitQNotify = false;
	start();
};

void NBFileSystemWatcher::stopWatch() {

	__quitQNotify = true;
	__watching = false;
};

void NBFileSystemWatcher::run() {

	bool __watchPathDeleted = false;

	while ( not __quitQNotify ) {

		while ( __watchPathDeleted )
			usleep( 100 );

		WD = inotify_add_watch( inotifyFD, qPrintable( watchPath ), IN_CREATE | IN_MODIFY | IN_DELETE | IN_DELETE_SELF );
		if ( WD == -1 ) {
			qCritical() << "Couldn't add watch: " << qPrintable( watchPath );
			close( inotifyFD );
			continue;
		}

		__watchChanging = false;
		int length = 0, i = 0;

		while( not __watchChanging ) {
			i = 0;
			length = read( inotifyFD, buffer, BUF_LEN );

			if ( length < 0 )
				qDebug() << "Could not read changes.";

			if ( __watchChanging ) {
				inotify_rm_watch( inotifyFD, WD );
				break;
			}

			while ( i < length ) {
				if ( __quitQNotify ) {
					inotify_rm_watch( inotifyFD, WD );
					return;
				}

				if ( __watchChanging ) {
					inotify_rm_watch( inotifyFD, WD );
					break;
				}

				struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

				if ( event->mask & IN_DELETE_SELF ) {
					__watchPathDeleted = true;

					emit watchPathDeleted();

					/*
						*
						* The folder which was being watched was itself deleted, so we have
						* nothing else to watch for. So break out of the while read loop
						*
					*/
					inotify_rm_watch( inotifyFD, WD );
					break;
				}

				if ( event->mask & IN_CREATE ) {
					if ( event->mask & IN_ISDIR )
						emit dirCreated( event->name );

					else
						emit fileCreated( event->name );
				}

				if ( event->mask & IN_MODIFY ) {
					if ( event->mask & IN_ISDIR )
						emit dirChanged( event->name );

					else
						emit fileChanged( event->name );
				}

				if ( event->mask & IN_DELETE ) {
					if ( event->mask & IN_ISDIR )
						emit dirDeleted( event->name );

					else
						emit fileDeleted( event->name );
				}

				i += EVENT_SIZE + event->len;
			}

			if ( __quitQNotify ) {
				inotify_rm_watch( inotifyFD, WD );
				return;
			}

			if ( __watchChanging ) {
				inotify_rm_watch( inotifyFD, WD );
				break;
			}
		}

		inotify_rm_watch( inotifyFD, WD );
	}
};

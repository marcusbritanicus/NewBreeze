/*
	*
	* NBFileSystemWatcher.cpp - INotify Class for Qt4
	*
*/

#include <NBFileSystemWatcher.hpp>
#include <poll.h>

NBFileSystemWatcher::NBFileSystemWatcher() : QThread() {

	mStopWatch = false;

	monitor.wd = -1;
	cookie = -1;
	mask = -1;

	inotify_fd = inotify_init();
	if ( inotify_fd < 0 ) {
		qCritical() << "Failed initialize inotify";
		emit watchFailed();
	}
};

NBFileSystemWatcher::~NBFileSystemWatcher() {

	if ( monitor.wd >= 0 )
		stopWatch();
};

void NBFileSystemWatcher::startWatch( QString wPath ) {

	if ( not wPath.endsWith( "/" ) )
		wPath += "/";

	monitor.path = strdup( wPath.toLatin1().data() );
	monitor.wd = inotify_add_watch( inotify_fd, monitor.path, IN_ALL_EVENTS );
	if ( monitor.wd < 0 ) {
		qCritical() << "Failed to add watch:" << monitor.path;
		qCritical() << "inotify_add_watch(...) returned error:" << errno << strerror( errno );
		emit watchFailed();
	}

	mStopWatch = false;
	start();
};

void NBFileSystemWatcher::stopWatch() {

	mStopWatch = true;

	if ( monitor.wd >= 0 ) {
		inotify_rm_watch( inotify_fd, monitor.wd );
		free( monitor.path );

		monitor.wd = -1;
	}
	close( inotify_fd );

	/* Quit the thread */
	quit();

	/* Wait for the thread to finish */
	wait();
};

void NBFileSystemWatcher::run() {

	struct pollfd fds[ 1 ];
	fds[ 0 ].fd = inotify_fd;
	fds[ 0 ].events = POLLIN;

	while ( not mStopWatch ) {
		/* End watch */
		if ( mStopWatch )
			break;

		/* Block until there is something to be read */
		if ( poll( fds, 1, -1 ) < 0 )
			qCritical() << "Couldn't poll():" << strerror( errno );

		/* Inotify event received? */
		if ( fds[ 0 ].revents & POLLIN and not mStopWatch ) {
			char buffer[INOTIFY_BUFFER_SIZE];
			size_t length;

			/* Read from the FD. It will read all events available up to * the given buffer size. */
			if ( ( length = read( fds[ 0 ].fd, buffer, INOTIFY_BUFFER_SIZE ) ) > 0 ) {
				struct inotify_event *event;
				size_t i = 0;

				while ( i < length ) {
					if ( mStopWatch )
						break;

					event = ( struct inotify_event * ) &buffer[ i ];
					i += EVENT_SIZE + event->len;

					processEvent( event );
				}
			}
		}
	}
};

/* Eventually we will emit signals from here, corresponding to each event */
void NBFileSystemWatcher::processEvent( struct inotify_event *event ) {

	if ( monitor.wd == event->wd ) {
		if ( ( mask == IN_MOVED_FROM ) and ( cookie != event->cookie ) ) {
			mask = -1;
			cookie = -1;

			emit nodeDeleted( oldName );
		}

		if ( event->mask & IN_ACCESS ) {
			mask = event->mask;
			cookie = event->cookie;
			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_ATTRIB ) {
			mask = event->mask;
			cookie = event->cookie;
			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_OPEN ) {
			mask = event->mask;
			cookie = event->cookie;
			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_CLOSE_WRITE ) {
			mask = event->mask;
			cookie = event->cookie;
			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_CLOSE_NOWRITE ) {
			mask = event->mask;
			cookie = event->cookie;
			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_CREATE ) {
			mask = event->mask;
			cookie = event->cookie;
			emit nodeCreated( QString( monitor.path ) + event->name );
		}

		if ( event->mask & IN_MODIFY ) {
			mask = event->mask;
			cookie = event->cookie;
			emit nodeChanged( QString( monitor.path ) + event->name );
		}

		if ( event->mask & IN_DELETE ) {
			mask = event->mask;
			cookie = event->cookie;
			emit nodeDeleted( QString( monitor.path ) + event->name );
		}

		if ( event->mask & IN_DELETE_SELF ) {
			mask = event->mask;
			cookie = event->cookie;

			emit watchPathDeleted();
			stopWatch();
		}

		if ( event->mask & IN_MOVE_SELF ) {
			mask = event->mask;
			cookie = event->cookie;

			emit watchPathDeleted();
			stopWatch();
		}

		if ( event->mask & IN_MOVED_FROM ) {
			mask = IN_MOVED_FROM;
			cookie = event->cookie;

			oldName = QString( monitor.path ) + event->name;
		}

		if ( event->mask & IN_MOVED_TO ) {
			/* If we have the same cookie, and last event was IN_MOVED_FROM */
			if ( ( cookie == event->cookie ) and ( mask == IN_MOVED_FROM ) ) {
				mask = IN_MOVED_TO;
				cookie = -1;
				emit nodeRenamed( oldName, QString( monitor.path ) + event->name );
			}

			/* If the cookie is different or mask is not IN_MOVED_FROM */
			else {
				mask = IN_MOVED_TO;
				cookie = -1;
				emit nodeCreated( QString( monitor.path ) + event->name );
			}
		}

		return;
	}
};

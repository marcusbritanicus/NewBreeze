/*
	*
	* NBFileSystemWatcher.cpp - Advanced version of QFileSystemWatcher for Qt5
	*
*/

#include "NBFileSystemWatcher.hpp"
#include "Global.hpp"

#include <sys/inotify.h>

#define MAX_EVENTS				1024
#define EVENT_SIZE				( sizeof ( struct inotify_event ) )
#define BUF_LEN	 				( MAX_EVENTS * ( EVENT_SIZE + NAME_MAX + 1 ) )

NBFileSystemWatcher::NBFileSystemWatcher() : QThread() {

	inotifyFD = inotify_init();
	if ( inotifyFD < 0 ) {
		qCritical() << "Failed initialize inotify";
		inotifyFailed();
	}

	expireTimer = new QBasicTimer();
	expireTimer->start( 50, Qt::PreciseTimer, this );
};

NBFileSystemWatcher::~NBFileSystemWatcher() {

	__stopWatch = true;

	expireTimer->stop();
	delete expireTimer;

	// We should now close all the open watches
	for( int key: wdPathHash.keys() ) {
		wdPathHash.remove( key );
		wdModeHash.remove( key );
		inotify_rm_watch( inotifyFD, key );
	}

	// Close the inotify instance
	close( inotifyFD );
};

void NBFileSystemWatcher::addWatch( QString wPath, Mode wMode ) {

	/**
	 * Remove the trailing '/' from paths
	 */
	while ( ( wPath != "/" ) and wPath.endsWith( "/" ) )
		wPath.chop( 1 );

	// Add a new watch
	int WD = inotify_add_watch( inotifyFD, wPath.toUtf8().data(), IN_ALL_EVENTS );
	if ( WD == -1 ) {
		qWarning() << "Couldn't add watch: " << wPath.toUtf8().constData();
		emit watchFailed( wPath );
	}

	wdPathHash[ WD ] = wPath;
	wdModeHash[ WD ] = ( isDir( wPath ) ? wMode : PathOnly );

	/**
	 * Add more watches based on wMode
	 */
	if ( isDir( wPath ) ) {
		if ( wMode == Recursive ) {
			QDirIterator it( wPath, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden, QDirIterator::Subdirectories );
			while( it.hasNext() ) {
				QString entry( it.next() );
				WD = inotify_add_watch( inotifyFD, entry.toUtf8().data(), IN_ALL_EVENTS );

				if( WD != -1 ) {
					wdPathHash[ WD ] = entry;
					wdModeHash[ WD ] = ( it.fileInfo().isDir() ? Recursive : PathOnly );
				}
			}
		}

		else if ( wMode == Contents ) {
			QDirIterator it( wPath, QDir::Files | QDir::System | QDir::Hidden );
			while( it.hasNext() ) {
				QString entry( it.next() );
				WD = inotify_add_watch( inotifyFD, entry.toUtf8().data(), IN_ALL_EVENTS );

				if( WD != -1 ) {
					wdPathHash[ WD ] = entry;
					wdModeHash[ WD ] = PathOnly;
				}
			}
		}
	}
};

void NBFileSystemWatcher::removeWatch( QString path ) {

	for( int key: wdPathHash.keys() ) {
		if ( wdPathHash.value( key ).startsWith( path ) ) {
			wdPathHash.remove( key );
			wdModeHash.remove( key );
			inotify_rm_watch( inotifyFD, key );
		}
	}
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
            char buffer[ BUF_LEN ] = { 0 };
			length = read( inotifyFD, buffer, BUF_LEN );

			i = 0;
			// If for some reason length < 0, continue with trying to read.
			if ( length < 0 )
				continue;

			while ( i < length ) {
				struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];

                /* WD of the current event */
                int curWD = event->wd;

				// If this wd does not exist in our hash
				if ( not wdPathHash.keys().contains( curWD ) )
					break;

                QString node = wdPathHash.value( curWD );
				QString path = node + ( event->len ? QString( "/%1" ).arg( event->name ) : "" );

                /**
                 * IN_DELETE_SELF means the node which was being watched was deleted
                 * So we should delete it from the
                 */
				if ( event->mask & IN_DELETE_SELF ) {
					emit nodeDeleted( node );
					removeWatch( node );
                }

				// New file was created inside one of the directories we were watching
				else if ( ( event->mask & IN_CREATE ) ) {

					emit nodeCreated( path );
					switch ( wdModeHash.value( curWD ) ) {

						case Recursive: {
							addWatch( path, Recursive );
							break;
						}

						case Contents: {
							addWatch( path, Contents );
							break;
						}

						default: {
							break;
						}
					}
                }

				// A file was modified inside one of the directories we were watching
				else if ( event->mask & IN_MODIFY ) {

					/**
					 * If we have a folder, and we're watching it with PathOnly mode,
					 * then is the only chance to intimate the user that the file was modified
					 */
					if ( isDir( node ) and wdModeHash.value( curWD ) == PathOnly )
						emit nodeChanged( path );

					/**
					 * Else if, we have a file, we will right away intimate the user
					 * that the file was modified
					 */
					else if ( not isDir( node ) )
						emit nodeChanged( node );
				}

				// A file was deleted inside one of the directories we were watching
				else if ( ( event->mask & IN_DELETE ) ) {

					emit nodeDeleted( path );
					removeWatch( path );
				}

				// A file was moved FROM one of the directories we were watching
				else if ( ( event->mask & IN_MOVED_FROM ) ) {

					cookiePathHash[ event->cookie ] = path;
					cookieTimeHash[ event->cookie ] = QDateTime::currentMSecsSinceEpoch();

					/**
					 * Remove this path from watch and put it into pending renames
					 */
					removeWatch( path );

					/**
					 * Further watch of this node will happen only if this directory was
					 * watched in Recursive or Contents mode.
					 */
					switch( wdModeHash.value( curWD ) ) {
						case Recursive: {
							pendingRenames[ event->cookie ] = Recursive;
							break;
						}

						case Contents: {
							/* If we have a folder, ignore it */
							if ( not isDir( path ) )
								pendingRenames[ event->cookie ] = PathOnly;

							break;
						}

						case PathOnly:
						default: {
							break;
						}
					}
				}

				// A file was moved TO one of the directories we were watching
				else if ( ( event->mask & IN_MOVED_TO ) ) {

					// Previously, we received a IN_MOVED_FROM event with this cookie.
					if ( cookiePathHash.keys().contains( event->cookie ) ) {
						/** Add a watch and then emit renamed signal */
						addWatch( path, pendingRenames.value( event->cookie ) );
						emit nodeRenamed( cookiePathHash.value( event->cookie ), path );

						cookiePathHash.remove( event->cookie );
						cookieTimeHash.remove( event->cookie );
						pendingRenames.remove( event->cookie );
					}

					// This node is being moved from somewhere outside our watches
					else {
						addWatch( path, wdModeHash.value( curWD ) );
						emit nodeCreated( path );
					}
				}

				// A file/directory we were watching was moved
				else if ( ( event->mask & IN_MOVE_SELF ) ) {
					// We will remove the watch, and create a delete event
					emit nodeDeleted( node );
                    wdPathHash.remove( curWD );
                    wdModeHash.remove( curWD );

					removeWatch( node );
				}

				i += EVENT_SIZE + event->len;
			}
		}
	}
};

void NBFileSystemWatcher::timerEvent( QTimerEvent *tEvent ) {

	if ( expireTimer->timerId() == tEvent->timerId() ) {
		time_t curTime = QDateTime::currentMSecsSinceEpoch();
		for( int cookie: cookieTimeHash.keys() ) {
			if ( curTime - cookieTimeHash.value( cookie ) > 500 ) {
				QString path = cookiePathHash.value( cookie );
				emit nodeDeleted( path );

				cookiePathHash.remove( cookie );
				cookieTimeHash.remove( cookie );
				pendingRenames.remove( cookie );
			}
		}
	}

	QThread::timerEvent( tEvent );
};

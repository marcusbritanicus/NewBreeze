/*
	*
	* NBFileSystemWatcher.hpp - NBFileSystemWatcher Class Header
	*
*/

#pragma once

#include <Global.hpp>

/* Size of buffer to use when reading inotify events */
#define MAX_EVENTS				1024
#define LEN_NAME				256
#define EVENT_SIZE				( sizeof ( struct inotify_event ) )
#define INOTIFY_BUFFER_SIZE 	( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ) )

class NBFileSystemWatcher : public QThread {
	Q_OBJECT

	public:
		enum Type {
			DIRECTORY,
			FILE
		};

		NBFileSystemWatcher();
		~NBFileSystemWatcher();

		void setWatchPath( QString );

		void startWatch();
		void restartWatch();
		void stopWatch();

	protected:
		void run();

	private:
		/* A function to check for all the event masks */
		void processEvent( struct inotify_event *event );

		/* Structure to keep track of monitored directories */
		struct monitor_t {
			/* Path of the directory */
			char *path;
			/* inotify watch descriptor */
			int wd;
		} monitor;

		int inotify_fd;
		bool mStopWatch;
		bool mWatching;

		uint32_t cookie;
		uint32_t mask;

		QString oldName;

	Q_SIGNALS :
		void nodeCreated( QString );
		void nodeDeleted( QString );
		void nodeChanged( QString );
		void nodeRenamed( QString, QString );

		void watchPathDeleted();

		void watchFailed();
};

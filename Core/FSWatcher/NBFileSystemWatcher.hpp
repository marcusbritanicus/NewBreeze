/*
	*
	* NBFileSystemWatcher.hpp - NBFileSystemWatcher Class Header
	*
*/

#pragma once
#ifndef NBFILESYSTEMWATCHER_HPP
#define NBFILESYSTEMWATCHER_HPP

#include <Global.hpp>

#define MAX_EVENTS				1024
#define LEN_NAME				256
#define EVENT_SIZE				( sizeof ( struct inotify_event ) )
#define BUF_LEN	 				( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME ) )

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
		void waitForWatchEnded();

		int inotifyFD;
		int WD;
		char buffer[ BUF_LEN ];

		QString watchPath;
		QString originalName;
		int prevEvent;

		bool __stopWatch;

	Q_SIGNALS :
		void nodeCreated( QString );
		void nodeDeleted( QString );
		void nodeChanged( QString );
		void nodeRenamed( QString, QString );

		void watchPathDeleted();

		void watchFailed();
};

#endif

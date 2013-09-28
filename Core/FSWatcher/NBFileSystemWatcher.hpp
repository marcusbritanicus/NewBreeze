/*
	*
	* NBFileSystemWatcher.hpp - NBFileSystemWatcher Class Header
	*
*/

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
		void stopWatch();

	protected:
		void run();

	private:
		void waitForWatchEnded();

		int inotifyFD;
		int WD;
		char buffer[ BUF_LEN ];

		QString watchPath;

		bool __quitQNotify;
		bool __watchChanging;
		bool __watching;
		bool __canStartNewWatch;

	Q_SIGNALS :
		void fileCreated( QString );
		void fileDeleted( QString );
		void fileChanged( QString );

		void dirCreated( QString );
		void dirDeleted( QString );
		void dirChanged( QString );

		void watchPathDeleted();
};

#endif

/*
	*
	* NBFileSystemWatcher.hpp - NBFileSystemWatcher Class Header
	*
*/

#pragma once

#include <QHash>
#include <QThread>
#include <QBasicTimer>

class NBFileSystemWatcher : public QThread {
	Q_OBJECT;

	public:
        /**
         * How should we be watching the added paths
         */
        enum Mode {
            /** Watch the directories recursively
             * So, a folder, it's contents, and subfolders and their contents
             * will be watched.
             * Any newly added files/folders will also be watched
             * WARNING: Use with extreme care!
             */
            Recursive = 0x2A16C7,

            /** Watch the directory and it's contents only
             * Don't watch the events of the subfolders. However, addition and
             * deletion of subfolders will be reported.
             * All the newly added files will be watched for all events
             */
            Contents,

            /** Watch the added path only
             * Watch the added path only. Newly created paths (files/folders)
             * will not be added to the watch automatically. However, they can
             * be added manually. Note that file/folder creation/deletion events
             * inside a watched directory will be reported.
             */
            PathOnly,
        };

		NBFileSystemWatcher();
		~NBFileSystemWatcher();

		void addWatch( QString, Mode watchMode = PathOnly );
		void removeWatch( QString );

		void startWatch();
		void restartWatch();
		void stopWatch();

	protected:
        void timerEvent( QTimerEvent* ) override;

	private:
		void run();

		int inotifyFD = -1;

        /**
         * Hash to store the wd and path of the added watches.
         * inotify sorts the events based on wd.
         */
		QHash<int, QString> wdPathHash;

        /**
         * Hash to store the wd and mode of the added watches.
         * inotify sorts the events based on wd. Based on the
         * mode we will handle newly added nodes.
         */
		QHash<int, Mode> wdModeHash;

        /**
         * Cookie hashes to handle IN_MOVED_FROM/IN_MOVED_TO events.
         * We will store the cookie, and the IN_MOVED_FROM path in one hash
         * and the cookie and received time in another hash.
         * Once we receive the IN_MOVED_TO event with the same cookie,
         * We can emit a rename event. If we do not get a IN_MOVED_TO
         * event, in 500ms, we will consider it as deleted.
         */
        QHash<int, QString> cookiePathHash;
        QHash<int, time_t> cookieTimeHash;

        /**
         * Flag to stop all the watches
         */
		bool __stopWatch = false;

        /**
         * Timer to discard the IN_MOVED_FROM events when they expire.
         */
        QBasicTimer *expireTimer;

        /**
         * A list of string to
         */
        QHash<int, Mode> pendingRenames;

	Q_SIGNALS :
		void nodeCreated( QString );
		void nodeDeleted( QString );
		void nodeChanged( QString );

		void nodeRenamed( QString /* old */, QString /* new */ );

		void watchFailed( QString );

        void inotifyFailed();
};

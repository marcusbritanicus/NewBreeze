/*
	*
	* NBProcess.hpp - NBProcess.cpp header
	*
*/

#pragma once

#include <Global.hpp>

typedef struct NBProgress_t {
	QString sourceDir;
	QString targetDir;
	quint64 totalBytes;
	quint64 totalBytesCopied;
	QString currentFile;
	quint64 fileBytes;
	quint64 fileBytesCopied;
} NBProgress;

class NBProcess : public QThread {
	Q_OBJECT

	public:
		enum ProcessType {
			Copy,
			Move,
			Delete,
			Trash
		};

		NBProcess( QObject *parent );

		virtual NBProgress progress();
};

class NBIOProcess : public NBProcess {
	Q_OBJECT

	public:
		NBIOProcess( QStringList sources, QString target, NBProcess::ProcessType, NBProgress *progress )

		// Set the sources - What to Copy/Move/ACopy
		void setSources( QStringList );

		// Set the target - Where to Copy/Move/ACopy
		void setTarget( QString );

		// The list of nodes which could not be copied/moved/archived
		QStringList errors();

		// Cancel the IO Operation
		void cancel();

		// Pause the IO Operation
		void pause();

		// Resume the paused IO
		void resume();

	protected:
		void run();

	private:
		// Things to be done before IO begins like computing sizes
		void preIO();

		// Compute the size of a directory
		void getDirSize( QString );

		// Copy a file
		void copyFile( QString );

		// Copy a folder
		void copyDir( QString );

		// Create a directory tree - A directory with sub-directory, sub-sub-directory, and so on
		void mkpath( QString, QFile::Permissions );

		QStringList errorNodes;
		QStringList sourceList;
		QString targetDir;

		QString jobID;

		bool wasCanceled;
		bool isPaused;

		NBIOMode::Mode mode;

		NBProgress mProgress;
};

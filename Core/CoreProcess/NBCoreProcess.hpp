/*
	*
	* NBCoreProcess.hpp - NBCoreProcess.cpp header
	*
*/

#pragma once

#include <Global.hpp>

class NBProgress {
	QStringList sourceList;
	QString targetDir;
	quint64 totalBytes;
	quint64 totalBytesCopied;
	QString currentFile;
	quint64 fileBytes;
	quint64 fileBytesCopied;
};

class NBCoreProcess : public QThread {
	Q_OBJECT

	public:
		enum ProcessType {
			Copy,
			Move,
			Permisson,
			ArchiveRead,
			ArchiveWrite,
			Delete,
			Trash
		};

		NBCoreProcess( QObject *parent ) {
		};

		virtual NBProgress progress();
};

class NBIOProcess : public NBCoreProcess {
	Q_OBJECT

	public:
		NBIOProcess( QStringList sources, QString target, NBCoreProcess::ProcessType )

		// Set the sources - What to Copy/Move/ACopy
		void setSources( QStringList );

		// Return the source list
		QStringList sources();

		// Set the target - Where to Copy/Move/ACopy
		void setTarget( QString );

		// Return the target
		QString target();

		// Set the mode - How to do the IO: Copy or Move or ACopy
		void setIOMode( NBIOMode::Mode );

		// Return the mode of IO
		NBIOMode::Mode ioMode();

		// The list of nodes which could not be copied/moved/archived
		QStringList errors();

		// Cancel the IO Operation
		void cancel();

		// Pause the IO Operation
		void pause();

		// Resume the paused IO
		void resume();

		// Progress
		NBProgress progress();

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

class NBPermissionSettings : public NBCoreProcess {

	public:
		NBPermissionSettings( QObject *parent );
};

/*
	*
	* NBFileCopy.hpp - NewBreeze FileCopy Class Header
	*
*/

#pragma once
#ifndef NBFILEIO_HPP
#define NBFILEIO_HPP

#include <Global.hpp>
#include <sys/statvfs.h>
#include <NBTools.hpp>
#include <NBDeviceInfo.hpp>

class NBFileIO : public QObject {
	Q_OBJECT

	public:
		// Initializer
		NBFileIO();

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

		// Cancel the IO Operation
		void cancel();

		// Pause the IO Operation
		void pause();

		// Resume the paused IO
		void resume();

		// ID - Identity of this IO process
		const QString id();

		// What is the result of the IO
		int result();

		// The list of nodes which could not be copied/moved/archived
		QStringList errors();

		// Total bytes to be copied
		quint64 totalSize;

		// Bytes copied so far
		quint64 copiedSize;

		// Size of the current file
		quint64 fTotalBytes;

		// Bytes copied of the current file
		quint64 fWritten;

		// The currently being written file
		QString ioTarget;

	public slots:
		// Start the IO
		void performIO();

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

		bool wasCanceled = false;
		bool isPaused = false;

		NBIOMode::Mode mode;

	Q_SIGNALS:
		void IOComplete();
};

#endif

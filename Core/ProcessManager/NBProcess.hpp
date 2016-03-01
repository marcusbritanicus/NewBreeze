/*
	*
	* NBProcess.hpp - NBProcess.cpp header
	*
*/

#pragma once

#include <Global.hpp>

namespace NBProcess {

	enum Type {
		Copy = 0x5CF670,				// Copy file/dir
		Move,							// Move file/dir
		Delete,							// Delete from disk
		Trash,							// Move to trash
		Properties						// Set file properties
	};

	enum State  {
		Starting = 0x7A242A,			// Listing the sources
		Started,						// Process is on going
		Paused,							// Process is paused
		Canceled,						// Process was cancelled
		Complete						// Process is complete (with/without errors)
	};

	typedef struct NBProgress_t {
		/* The source directory */
		QString sourceDir;

		/* The target directory */
		QString targetDir;

		/* Total bytes to be copied */
		quint64 totalBytes;

		/* Total bytes already copied */
		quint64 totalBytesCopied;

		/* Current file name */
		QString currentFile;

		/* Current file size */
		quint64 fileBytes;

		/* Current file bytes already copied */
		quint64 fileBytesCopied;

		/* Text to be displayed with the progress bar */
		/* Mainly used for preIO stage */
		QString progressText;

		/* Type: Copy, Move, Delete, Trash */
		Type type;

		/* State: Starting, Started, Paused, Canceled, */
		State state;
	} Progress;

};

class NBAbstractProcess : public QThread {
	Q_OBJECT

	public:
		virtual QStringList errors() = 0;
		virtual void cancel() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;

	protected:
		/* Force the subclass to implement the function run */
		virtual run() = 0;
};

class NBIOProcess : public NBAbstractProcess {
	Q_OBJECT

	public:
		NBIOProcess( QStringList sources, QString target, NBProcess::Progress *progress )

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

		// Copy a file
		void copyFile( QString );

		// Copy a folder
		void copyDir( QString );

		QStringList errorNodes;
		QStringList sourceList;
		QString targetDir;

		bool mCanceled;
		bool mPaused;

		NBProcess::Progress *mProgress;
};

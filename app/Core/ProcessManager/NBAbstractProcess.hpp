/*
	*
	* NBAbstractProcess.hpp - NBAbstractProcess.cpp header
	*
	* NBProgress is a struct that contains all the necessary details for showing a progress dialog.
	*
	*   - sourceDir               The directory where all the source files live
	*   - targetDir               The directory to where the sources will be copied/moved
	*   - totalBytes              Total size of all sources
	*   - totalBytesCopied        Volume of IO that has taken place (bytes copied to the target)
	*   - fileBytes               The size of the file currently being processed
	*   - fileBytesCopied         Volume of the file already copied (bytes of the file copied)
	*   - progressText            Text to be shown by the NBProgressBar (optional)
	*   - type                    Type of process (See NBProcess::Type)
	*   - state                   Type of process (See NBProcess::State)
	*
	* Note: Path for all the sources that need to be processed must be relative to NBProgress::sourceDir
	* Example:
	*   If we want to copy all the files living in /usr/share/, then
	*       sourceDir will be /usr/share/
	*       sources will be alsa, applications, aspell, ..., xsessions, zoneinfo and so on.
	*
	* Note:
	*   I have attempted to copy all the types of nodes including character special, block devices, fifo, etc using mknod
	*   Despite this, I have not found a way to copy sockets. Perhaps it is meaningless to say such a thing as
	*   'copy a socket'
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
		NotStarted = 0x7A242A,			// We are yet to begin doing anything with this (for delayed start, etc)
		Starting,						// Listing the sources
		Started,						// Process is on going
		Paused,							// Process is paused
		Canceled,						// Process was cancelled
		Completed						// Process is complete (with/without errors)
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

		/* When did this process start */
		QTime startTime;

		/* Text to be displayed with the progress bar */
		QString progressText;

		/* Type: Copy, Move, Delete, Trash */
		NBProcess::Type type;

		/* State: Starting, Started, Paused, Canceled, */
		NBProcess::State state;
	} Progress;
};

class NBAbstractProcess : public QThread {
	Q_OBJECT

	public:
		virtual QStringList errors() = 0;
		virtual void cancel() = 0;
		virtual void pause() = 0;
		virtual void resume() = 0;

		virtual QStringList nodes() = 0;

		virtual bool canUndo() = 0;
		virtual void undo() = 0;

		// virtual bool canRedo() = 0;
		// virtual void redo() = 0;

	protected:
		/* Force the subclass to implement the function run */
		virtual void run() = 0;

	Q_SIGNALS:
		/* Signals completion, with error list as the positional argument */
		void completed( QStringList );

		/* Signals cancelation, with errors encountered so far as the positional argument */
		void canceled( QStringList );

		/* Error: No write permission */
		void noWriteAccess();

		/* Error: Target does not have enough space */
		void noSpace();
};

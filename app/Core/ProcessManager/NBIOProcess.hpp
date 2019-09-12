/*
	*
	* NBIOProcess.hpp - NBIOProcess.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBAbstractProcess.hpp"

class IODialog;

class NBIOProcess : public NBAbstractProcess {
	Q_OBJECT

	public:
		NBIOProcess( QStringList sources, NBProcess::Process *process, QWidget *parent );

		// The list of nodes which could not be copied/moved/archived
		QStringList errors();

		/* We need this because we cannot create dialogs from within the thread */
		QMessageBox::StandardButton resolution = QMessageBox::NoButton;

	public Q_SLOTS:
		// Cancel the IO Operation
		void cancel();

		// Pause/Resume the IO Operation
		void pause();

		// Resume the paused IO
		void resume();

		// Nodes
		QStringList nodes() {

			return sourceList;
		};

	protected:
		void run();

	private:
		// Things to be done before IO begins like computing sizes
		bool preIO();

		// List the directory contents, and get the size
		void processDirectory( QString );

		// Copy a file
		void copyFile( QString );

		/* Get new filename */
		QString newFileName( QString );

		/* This will store the 'sources' list passed to the process  */
		QStringList origSources;

		/* All the not directory nodes in the sources will be listed here */
		QStringList sourceList;

		/* List of files which should not be deleted */
		QStringList keepList;

		QStringList errorNodes;

		bool mCanceled;
		bool mPaused;

		short int mResolveConflict = 0;

		NBProcess::Process *mProcess;

	Q_SIGNALS:
		/* Signals completion, with error list as the positional argument */
		void completed( QStringList );

		/* Signals cancelation, with errors encountered so far as the positional argument */
		void canceled( QStringList );

		/* Resolve conflicts with existing files */
		void resolveConflict( QString );

		/* Error: No write permission */
		void noWriteAccess();

		/* Error: Target does not have enough space */
		void noSpace();
};

class ConflictDialog : public QMessageBox {
	Q_OBJECT

	public:
		static QMessageBox::StandardButton resolveConflict( QString file, QWidget *parent );
};

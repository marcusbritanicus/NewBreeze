/*
	*
	* NBDeleteProcess.hpp - NBDeleteProcess.cpp header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBAbstractProcess.hpp"

class NBDeleteProcess : public NBAbstractProcess {
	Q_OBJECT

	public:
		NBDeleteProcess( QStringList sources, NBProcess::Progress *progress );

		// The list of nodes which could not be copied/moved/archived
		QStringList errors();

		// Cancel the IO Operation
		void cancel();

		// Pause the IO Operation
		void pause();

		// Resume the paused IO
		void resume();

		// Can we undo this operation?
		bool canUndo();

		// Perform the undo
		void undo();

		// Nodes
		QStringList nodes() {

			return sourceList;
		};

	protected:
		void run();

	private:
		/* Delete the node (and its contents) forever */
		void deleteNode( QString path );

		/* Send the node (and its contents) to trash */
		void trashNode( QString path );

		/* Send the node (and its contents) to trash */
		void restoreNode( QString path );

		/* All the not directory nodes in the sources will be listed here */
		QStringList sourceList;

		QStringList errorNodes;

		bool mCanceled;
		bool mPaused;

		bool mUndo;

		NBProcess::Progress *mProgress;
};

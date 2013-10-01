/*
	*
	* NBFileCopy.hpp - NewBreeze FileCopy Class Header
	*
*/

#ifndef NBFILEIO_HPP
#define NBFILEIO_HPP

#include <Global.hpp>

class NBFileIO : public QObject, public QRunnable {
	Q_OBJECT

	public:
		// Init all the variables
		NBFileIO();

		// Mode: copy or move
		void setMode( NBIOMode::Mode mode );

		// Sources
		void setSources( QStringList );
		QStringList sources();

		// Target
		void setTarget( QString );
		QString target();

		// JobID
		void setJobID( QString );
		QString jobID();

		// Thread running
		void run();

		// Cancel the copy/move
		void cancelIO();

		// Wait for done
		bool wait( int );

		// Total number of nodes to be copied
		quint64 totalNodes();

		// Total bytes to be copied
		quint64 totalBytes();

		// Total bytes copied so far
		quint64 copiedBytes();

		// Current target file being copied
		QString currentFileCopied();

		// Bytes to be copied of this file
		quint64 cfileTotalBytes();

		// Bytes copied so far of this file
		quint64 cfileCopiedBytes();

		// Progress fraction: ( @ bytesCopied() ) /  ( @f totalBytes() )
		float totalProgress();
		float cfileProgress();

		// Puase and resume
		void pause();
		void resume();

	private:
		// Copy a directory
		bool copyDir( QString, QString );

		// Copy a file
		bool copyFile( QString, QString );

		// jobID
		QString m_jobID;

		// Variables to store the source list and target
		QStringList m_sourceList;
		QString m_target;

		// Variable for progressClick
		QString m_tgtFile;

		// Variable to store the mode
		NBIOMode::Mode m_mode;

		// List of falied i/o nodes
		QStringList failedNodes;

		// Pause flag
		bool paused;

		// Cancel flag
		bool canceled;

		// How many nodes to be copies
		quint64 nodes;
		// How many bytes to be copied in all
		quint64 totalBytesToBeCopied;
		// Hoy many bytes to be copied for this file
		quint64 cfileBytesToBeCopied;

		// Hoy many bytes copied in all
		quint64 totalBytesCopied;

		// Hoy many bytes copied for this file
		quint64 cfileBytesCopied;

	Q_SIGNALS:
		void complete( QString, QStringList );
};

#endif

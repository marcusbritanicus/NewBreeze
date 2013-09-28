/*
	*
	* NBFileCopy.hpp - NewBreeze FileCopy Class Header
	*
*/

#ifndef NBFILEIO_HPP
#define NBFILEIO_HPP

#include <Global.hpp>

class NBFileIO : public QThread {
	Q_OBJECT

	public:
		// Init all the variables
		NBFileIO( quint64 );

		// Mode: copy or move
		void setMode( NBIOMode::Mode mode );

		// Sources
		void setSources( QStringList );

		// Target
		void setTarget( QString );

		// JobID
		quint64 jobID();

		// Thread running
		void run();

		// Cancel the copy/move
		void cancelIO();

		// Percentage copied/moved
		float progress();

		// Puase and resume
		void pause();
		void resume();

	private:
		// Copy items in SourceList to Target
		void copyItems( QStringList, QString );

		// Move items from of SourceList to Target
		void moveItems( QStringList, QString );

		// Copy a directory
		bool copyDir( QString, QString );

		// Copy a file
		bool copyFile( QString, QString );

		// jobID
		quint64 m_jobID;

		// Variables to store the source list and target
		QStringList m_sourceList;
		QString m_target;

		// Class variable for progressClick
		QString m_tgtFile;

		// Variable to store the mode
		NBIOMode::Mode m_mode;

		// List of falied i/o nodes
		QStringList failedNodes;

		// Pause flag
		bool paused;

		// Cancel flag
		bool canceled;

		// How many bytes to be copied in all
		quint64 totalBytesToBeCopied;
		// Hoy many bytes to be copied for this file
		quint64 cfileBytesToBeCopied;

		// Hoy many bytes copied in all
		quint64 totalBytesCopied;

		// Hoy many bytes copied for this file
		quint64 cfileBytesCopied;

		// Progress timer
		QTimer *timer;

	public slots:
		void start();

	private slots:
		// Slot to produce progress signals
		void progressClick();

	Q_SIGNALS:
		// emit complete( jobID, errorNodes );
		void complete( quint64, QStringList );

		// emit copied( jobID, cFileName, cfileF, totalF );
		void copied( quint64, QString, float, float );

		// emit newNode( targetNode );
		void newNode( QString );

		// emit ioVolume( jobID, files, totalBytesToBeCopied );
		void ioVolume( quint64, quint64, quint64 );
};

#endif

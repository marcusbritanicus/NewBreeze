/*
	*
	* NBFileCopy.hpp - NewBreeze FileCopy Class Header
	*
*/

#ifndef NBFILEIO_HPP
#define NBFILEIO_HPP

#include <Global.hpp>

typedef struct _Job {

	// JobID - Unique string that identifies the job
	QString jobID = QString();

	// Source and target
	QStringList sources = QStringList();
	QString target = QString();

	// IO Mode
	NBIOMode::Mode mode = NBIOMode::Copy;

	// Sizes
	quint64 totalNodes = 0;
	quint64 totalBytes = 0;
	quint64 totalBytesCopied = 0;
	quint64 cfileBytes = 0;
	quint64 cfileBytesCopied = 0;

	// Current file
	QString currentFile = QString();

	// IO Operation Input
	bool canceled = false;
	bool paused = false;

	// IO Operation Output
	bool running = false;

	// Result
	bool completed = false;
	QStringList errorNodes = QStringList();
} Job;

void performIO( Job *job );

#endif

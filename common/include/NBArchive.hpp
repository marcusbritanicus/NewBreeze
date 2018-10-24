/*
	*
	* NBArchive.hpp - NBArchive.cpp header
	*
*/

#pragma once

// LibArchive
#include <archive.h>
#include <archive_entry.h>

// Qt Headers
#include "common.hpp"
#include <QtCore>

typedef struct {

	/* Name of the entry */
	QString name;

	/* Size of the entry */
	quint64 size;

	/* Type of the entry */
	int type;

	/* Stat equivalent */
	struct stat info;
} ArchiveEntry;

typedef QList<ArchiveEntry*> ArchiveEntries;

class NBCOMMON_DLLSPEC NBArchive : public QThread {
	Q_OBJECT

	public:
		NBArchive( QString );

		// Convinience Functions
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );

		/* Create an archive */
		void createArchive();

		/* Extract the archive */
		void extractArchive();

		/* Extract a named member of the archive */
		void extractMember( QString );

		/* List the contetns of the archive */
		ArchiveEntries listArchive();

		/* Exit status */
		int exitStatus();

	private:
		enum Mode {
			None				= 0xF650E7,
			Single,
			Container
		};

		enum Job {
			NoJob				= 0x25CEE9,
			CreateArchive,
			ExtractArchive,
			ExtractMember,
			ListArchive
		};

		/* Internal worker for copying data */
		int copyData( struct archive *ar, struct archive *aw );

		/* Set the archive filter format based on extensions */
		void setFilterFormat( QMimeType mType );

		/* Create an archive - Internal Worker */
		bool doCreateArchive();

		/* Extract the archive - Internal Worker */
		bool doExtractArchive();

		/* Extract a named member of the archive - Internal Worker */
		bool doExtractMember( QString );

		int mArchiveFilter;
		int mArchiveFormat;

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;

		ArchiveEntries memberList;
		bool readDone;
		int archiveType;

		/* What job are we doing? */
		int mJob;

		/* Is the job running? */
		bool isRunning;

		/* Exit status */
		int mExitStatus;

		/* Member to be extracted */
		QString extractedMember;

	protected:
		void run();

	Q_SIGNALS:
		void jobComplete();
		void jobFailed();
};

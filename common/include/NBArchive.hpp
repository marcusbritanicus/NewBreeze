/*
	*
	* NBArchive.hpp - NBArchive.cpp header
	*
*/

#pragma once

#include "common.hpp"

#include "NBTools.hpp"
#include "NBLibBZip2.hpp"
#include "NBLibGZip.hpp"
#include "NBLibLzma.hpp"
#include "NBLibLzma2.hpp"

typedef struct {

	/* Name of the entry */
	QString name;

	/* Size of the entry */
	quint64 size;

	/* Type of the entry */
	int type;

	/* Stat equivalent */
	struct stat *stat;

} ArchiveEntry;

typedef QList<ArchiveEntry*> ArchiveEntries;

class NBCOMMON_DLLSPEC NBArchive {

	public:
		NBArchive( QString );

		// Convinience Functions
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );

		/* Create an archive */
		void create();

		/* Extract the archive */
		int extract();

		/* Extract a named member of the archive */
		int extractMember( QString );

		ArchiveEntries list();

	private:
		/* Internal worker for copying data */
		int copyData( struct archive *ar, struct archive *aw );

		/* Set the archive filter format based on extensions */
		int setFilterFormat( struct archive *ar, QMimeType mType );

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;

		ArchiveEntries memberList;
		bool readDone;
};

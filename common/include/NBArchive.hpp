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
	struct stat info;
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

class NBArchiveThread : public QThread {
	Q_OBJECT

	public:
		inline NBArchiveThread( QString name, QChar mode, QString wDir, QString dest, QStringList files ) : QThread() {

			mName = name;
			mMode = mode;

			mDestDir = dest;
			mWorkDir = wDir;

			mFiles << files;
		};

	public Q_SLOTS:
		inline void run() {

			NBArchive *mArchive = new NBArchive( mName );

			switch( mMode.toLatin1() ) {
				// Compress
				case 'c': {

					mArchive->setWorkingDir( mWorkDir );
					mArchive->updateInputFiles( mFiles );
					mArchive->create();

					return;
				}

				// Decompress
				case 'd': {

					mArchive->setDestination( mDestDir );
					mArchive->extract();

					return;
				}

				// Decompress Member
				case 'm': {

					mArchive->setDestination( mDestDir );
					mArchive->extractMember( mFiles.at( 0 ) );

					return;
				}
			}

			emit complete( mName, mMode );
		};

	private:

		QString mName;
		QChar   mMode;

		QString mWorkDir;
		QString mDestDir;

		QStringList mFiles;

	Q_SIGNALS:
		/* complete( ArchiveName, ArchiveMode (c|d) */
		void complete( QString, QChar );
};

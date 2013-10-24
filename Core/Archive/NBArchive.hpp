/*
	*
	* NBArchive.hpp - NBArchive.cpp header
	*
*/

#ifndef NBARCHIVE_HPP
#define NBARCHIVE_HPP

#include <Global.hpp>

#include <NBLibBZip2.hpp>
#include <NBLibGZip.hpp>
#include <NBLibLzma.hpp>
#include <NBLibTar.hpp>
#include <NBLibZip.hpp>
#include <NBTools.hpp>

class NBArchive {

	public:
		enum Type {
			TAR,		// Tar Archive
			ZIP,		// Zip Compressed Archive
			BZ2,		// BZip2 Compressed Archive
			GZ,			// GZip Compressed Archive
			XZ,			// XZ Compressed Archive
			TZIP,		// Zip Compressed Tar Archive: .tar.zip - Unusual
			TBZ2,		// BZip2 Compressed Tar Archive: .tar.bz2, .tbz2
			TGZ,		// GZip Compressed Tar Archive: .tar.gz, .tgz
			TXZ			// XZ Compressed Tar Archive: .tar.xz, .txz =>THIS IS OUR DEFAULT TYPE
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBArchive( QString, NBArchive::Mode, NBArchive::Type );
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );
		void create();
		void extract();

		QStringList inputList;
		QString dest;
		QString src;

	private:
		void createTar();
		void createZip();
		void createBZ2();
		void createGZ();
		void createXZ();
		void createTZip();
		void createTBZ2();
		void createTGZ();
		void createTXZ();

		void extractTar();
		void extractZip();
		void extractBZ2();
		void extractGZ();
		void extractXZ();
		void extractTZip();
		void extractTBZ2();
		void extractTGZ();
		void extractTXZ();

		static QString archiveName;
		static NBArchive::Mode archiveMode;
		static NBArchive::Type archiveType;

		QString tempArchiveName;
};

#endif

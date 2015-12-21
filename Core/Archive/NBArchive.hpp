/*
	*
	* NBArchive.hpp - NBArchive.cpp header
	*
*/

#pragma once
#ifndef NBARCHIVE_HPP
#define NBARCHIVE_HPP

#include <Global.hpp>

#include <NBTools.hpp>
#include <NBLibBZip2.hpp>
#include <NBLibGZip.hpp>
#include <NBLibLzma.hpp>
#include <NBZipFile.hpp>
#include <NBTarFile.hpp>

class NBArchive {

	public:
		enum Type {
			TAR,		// Tar Archive: .tar,.tar.xz, .tar.gz, .tar.bz2
			ZIP,		// Zip Compressed Archive: .zip
			BZ2,		// BZip2 Compressed Archive: .bz2
			GZ,			// GZip Compressed Archive: .gz
			XZ,			// XZ Compressed Archive: .xz
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

	private:
		void createTar();
		void createZip();
		void createBZ2();
		void createGZ();
		void createXZ();

		void extractTar();
		void extractZip();
		void extractBZ2();
		void extractGZ();
		void extractXZ();

		static QString archiveName;
		static NBArchive::Mode archiveMode;
		static NBArchive::Type archiveType;

		QString tempArchiveName;

		QStringList inputList;
		QString dest;
		QString src;
};

#endif

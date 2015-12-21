/*
	*
	* libzip.hpp - libzip Header
	*
*/

#ifndef NBZIPFILE_HPP
#define NBZIPFILE_HPP

// Qt Headers
#include <QtCore>

// C++ Standard Library
#include <iostream>
#include <fstream>

// SystemWide Headers
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <zip.h>

#include <NBTools.hpp>

class ZipFile {

	public:
		enum OpenMode {
			Read = 0xB36A62,
			Write
		};

		/*
			*
			* Init the tar file:
			*     Arg1: Archive Name WITH extension
			*     Arg2: Archive Open Mode
			*     Arg3: Compression Filter
			*
		*/
		ZipFile( QString fileName, ZipFile::OpenMode mode );

		/* Check if the ZipFile was opened */
		bool isOpen();

		/* Read the archive */
		bool readArchive();

		/*
			*
			* Extract the contents of the archive:
			*     Arg1 is the target (optional)
			*     Arg2 is the name of the member (optional)
			*
		*/
		bool extract( QString target = QString(), QStringList members = QStringList() );

		/* Add a file to the archive: Arg is the name of the file */
		void addFile( QString );

		/* Add a dir to the archive: Arg is the name of the dir */
		void addDir( QString );

		/* Perform the archiving, after this no addFile/addDir calls are allowed */
		bool writeArchive();

		/* Close the tar file: discards all the unwriten data */
		void close();

	private :
		/* Used when reading archives */
		QStringList members;

		/* Used when writing archives */
		QStringList files;

		QString mFileName;
		ZipFile::OpenMode mMode;

		/* Zip File */
		struct zip *arc;

		/* Flag to check if the archive has been read once */
		bool mRead;
};

#endif

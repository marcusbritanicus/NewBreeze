/*
	*
	* NBTar.hpp - NBTar Header
	*
*/

#ifndef NBTar_HPP
#define NBTar_HPP

#include <QtCore>
#include <QCryptographicHash>

#include <fstream>

#include <bzlib.h>
#include <lzma.h>
#include <unistd.h>
#include <zlib.h>
#include <zip.h>
#include <dirent.h>
#include <utime.h>

#include <NBTools.hpp>
#include <NBTarHeader.hpp>

typedef QList<TarHeader *> TarHeaderList;
typedef QList<off_t> TarOffsetList;

class TarFile {

	public:
		enum OpenMode {
			Read = 0xB36A62,
			Write
		};

		enum Filter {
			NOFilter,				// Do not use a filter
			XZFilter,				// XZ Filter
			GZFilter,				// GZ Filter
			BZ2Filter,				// BZip2 Filter
			AUTOFilter,				// Determine the filter from the file name
		};

		/*
			*
			* Init the tar file:
			*     Arg1: Archive Name WITH extension
			*     Arg2: Archive Open Mode
			*     Arg3: Compression Filter
			*
		*/
		TarFile( QString fileName, TarFile::OpenMode mode, TarFile::Filter filter = TarFile::AUTOFilter );

		/* Check if the tarfile was opened */
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
		/* Get the Header of a member given by @arg */
		TarHeader* getMemberHeader( QString );

		bool writeArchiveXZ();
		bool writeArchiveGZ();
		bool writeArchiveBZ2();

		bool readArchiveXZ( QString );
		bool readArchiveGZ( QString );
		bool readArchiveBZ2( QString );

		/* Used when reading archives */
		TarHeaderList headers;
		TarOffsetList offsets;

		/* Used when writing archives */
		QStringList files;

		QString mFileName;
		TarFile::OpenMode mMode;
		TarFile::Filter mFilter;

		/* C++ File Object */
		std::fstream tarFile;

		/* Flag to check if the archive has been read once */
		bool mRead;
};

#endif

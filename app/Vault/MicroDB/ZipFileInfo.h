#ifndef ZIPFILEINFO_H
#define ZIPFILEINFO_H

#include "zipglobal.h"

#include <QObject>
#include <QDateTime>
#include <QFile>

/**
 * @brief This is a simple class to hold information about the archive
 *
 * \sa getInfo()
 */
class ZIP_EXPORT ZipInfo {
	public:
		ZipInfo();

		qint64  fileSize;       ///< The actual file of the archive
		qint64  packedSize;     ///< The total compressed size of all files
		qint64  unpackedSize;   ///< The total size of all files when extracted
		quint32 fileCount;      ///< The number of files stored in the archive
		qint32  version;        ///< The PKWARE Version made by. Major version/10, minor version % 10 (e.g. version = 45 which is 4.5)
		qint32  madeByOS;       ///< PKWARE OS made by. See PKWARE APPNOTE.TXT section 4.4.2.2
		bool    isZip64;        ///< true if the archive is in Zip64 format

		QString comment;        ///< The main archive comment

		// Stats during adding/extracting
		qint32 filesAdded;      ///< The total number of files added since open()
		qint32 filesExtracted;  ///< The total number of files extracted since open()
		qint32 filesDeleted;    ///< The total number of files deleted since open()
		qint32 errorCount;      ///< The total number of errors since open(). This can be reset by calling clearErrors()
};


class CentralDirFileHeader;

/**
 * @brief This class holds information about a file stored within the archive file.
 *
 * \sa findFind()
 */
class ZIP_EXPORT ZipFileInfo {

	public:
		ZipFileInfo();
		ZipFileInfo( CentralDirFileHeader* header );

		QString     filePath;           ///< The full path and filename as stored in the archive
		QString     comment;            ///< The comment assigned to this file
		QDateTime   lastModifiedDate;   ///< The last modifies date of the file
		quint32     crc32;              ///< The CRC checksum for the stored file
		quint64     compressedSize;     ///< The compressed side of the file in bytes
		quint64     uncompressedSize;   ///< The uncompressed (or original) size of the file
		quint32     compressionMethod;  ///< Pkware compression method (See PKWARE APPNOTE.TXT section 4.4.5)
		quint32     version;            ///< Version made by (See PKWARE APPNOTE.TXT section 4.4.2).
										/// Major version/10, minor version % 10 (e.g. version = 45 which is 4.5)
		quint32     madeByOS;           ///< PKWARE OS made by. See PKWARE APPNOTE.TXT section 4.4.2.2

		QFile::Permissions permissions; ///< The original file permission (e.g. read/write/execute etc.)
		quint8      attributes;         ///< DOS base file attributes (e.g archive, system, read only etc.)
										///< Bit 1 = Archive, Bit 2 = Hidden, Bit 4 = System, Bit 5 = folder, Bit 6 = Archive
		bool        isFolder;           ///< true if this entry is a folder only and not a file.
		bool        isEncrypted;        ///< true if this file is encrypted. See setPassword()
};

#endif // ZIPFILEINFO_H

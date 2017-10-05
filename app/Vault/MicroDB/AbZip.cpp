/****************************************************************************
**
** Copyright (C) 2016 Andy Bray
**
** This file is part of AbZip.
**
** AbZip is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AbZip is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AbZip.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "AbZip.h"
#include "abzip_p.h"
#include "ZipCentralDir.h"
#include "ZipDirIterator.h"
#include "LocalFileHeader.h"
#include "Compressor.h"
#include "StoreCompressor.h"
#include "encryption.h"
#include "Utils.h"

#include <QFile>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QTemporaryFile>
#include <QTextCodec>

#include <libgen.h>

using namespace ZipUtils;

AbZipPrivate::AbZipPrivate() {

	init();
};

AbZipPrivate::AbZipPrivate(const QString& _zipName) {

	init();
	zipName = _zipName;
};

AbZipPrivate::AbZipPrivate(QIODevice *_ioDevice) {

	init();
	ioDevice = _ioDevice;
};


void AbZipPrivate::init() {

	errorCode = AbZip::Ok;

	ioDevice = NULL;
	centralDir = new ZipCentralDir();

	filesAdded = filesExtracted = filesDeleted = errorCount = 0;

	// A selection of files we should just store
	storeTheseTypes << ".zip" << ".rar" << ".bz2" << ".gz" << ".7z" << ".s7z" << ".apk" << ".lzh"
					<< ".arc" << ".jar" << ".arj" << ".zlib" << ".cab" << ".tar" << "gzip" << ".xz"
					<< ".png" << ".mp3" << ".mpg" << ".wma" << ".avi" << ".mp4";

};

AbZipPrivate::~AbZipPrivate() {

	if(isOpen())
		close();
};


bool AbZipPrivate::open( AbZip::Mode mode, AbZip::ZipOptions options ) {

	if(isOpen())
	{
		qWarning("AbZip::open(): ZIP already opened");
		return false;
	};

	if (ioDevice == NULL)
	{
		if (zipName.isEmpty())
			return setError(AbZip::GeneralError, QT_TR_NOOP("Open failed, no archive file name or IO device specified!") );
		else
			ioDevice = new QFile(zipName);
	};

	switch(mode)
	{
	case AbZip::modeOpenRead:
		// Open for reading, extracting etc
		if (!ioDevice->open(QIODevice::ReadOnly))
			return setError(AbZip::OpenFileError, QT_TR_NOOP("Cannot open device for reading: ")+ioDevice->errorString() );
		break;

	case AbZip::modeCreateNew:
		if ( !backupOrDeleteCurrentZip( options ) )
			return false;
		// drop through..

	case AbZip::modeOpenWrite:
		// open file for writing
		if (!ioDevice->open(QIODevice::ReadWrite))
			return setError(AbZip::OpenFileError, QT_TR_NOOP("Cannot open device for writing: ")+ioDevice->errorString() );
		break;

	default:
		qWarning() << "AbZip::open(): invalid mode";
		if (!zipName.isEmpty())
			delete ioDevice;
		break;
	};

	// Always read the Central Directory
	if ( ioDevice->size() >= 22 && !centralDir->read( ioDevice, this ) )
	{
		// Attempt a recovery of CD
		if ( !rebuildCentralDirectory() )
			return setError(AbZip::InvalidArchive, QT_TR_NOOP("Error reading zip file. It may not be a valid zip file!") );
	};

	return true;
};

void AbZipPrivate::close() {

	if ( ioDevice && ioDevice->isOpen())
	{
		if ( ioDevice->openMode() == QIODevice::ReadWrite )
		{
			// Any deleted files to commit?
			if ( !commitDeletedFiles() )
			{
				flushAndClose();
			}
		};

		ioDevice->close();
	};

	// Free up the central directory
	centralDir->reset();

	// opened by name, need to delete the internal IO device
	if (!zipName.isEmpty() && ioDevice )
	{
		delete ioDevice;
		ioDevice = NULL;
	}
	filesAdded = filesExtracted = filesDeleted = errorCount = 0;
};

bool AbZipPrivate::backupOrDeleteCurrentZip( AbZip::ZipOptions options ) {


	QFile file( zipName );
	if ( !zipName.isEmpty() && file.exists( ) )
	{
		if ( options & AbZip::BackupExisting  )
		{
			QFileInfo fi(zipName);
			QString backupName;

			// Construct new name with an incremental number appended to the file name.
			// Keep upto 100 backups
			for( int i=1; i < 100; i++)
			{
				backupName = QString("%1/%2 (%3).%4").arg(fi.absolutePath())
													.arg(fi.baseName())
													.arg(i)
													.arg(fi.completeSuffix());
				if ( !QFile::exists( backupName ))
				{
					// Copy archive file  to this new name
					if ( !file.rename( backupName ) )
						return setError(AbZip::CopyError, QString(QT_TR_NOOP("Failed to rename archive '%1' to '%2' (%3)"))
										.arg( zipName )
										.arg(backupName)
										.arg(file.errorString()) );

					return true;
				}
			}
		};

		// Remove the current archive
		if ( !file.remove() )
			return setError(AbZip::DeleteError, QString(QT_TR_NOOP("Failed to remove archive '%1' (%2)"))
						.arg( zipName )
						.arg(file.errorString()) );

	};

	return true;
};

bool AbZipPrivate::checkAutoOpenWrite() {

	if ( !isOpen() )
	{
		// If not open then auto open for writing
		if ( !open(AbZip::modeOpenWrite ) )
			return false;
	}
	else
	{
		if ( ioDevice->openMode() == QIODevice::ReadOnly )
			return setError(AbZip::OpenInReadOnly, QT_TR_NOOP("Archive open in read mode, please close and open in write mode!") );
	}
	return isOpen();
};

bool AbZipPrivate::checkAutoOpenRead() {

	if ( !isOpen() )
	{
		// If not open then auto open for reading
		if ( !open(AbZip::modeOpenRead ) )
			return false;
	}
	return isOpen();
};

bool AbZipPrivate::isOpen()const
{
	return (ioDevice && ioDevice->isOpen() ) ? true : false;
};


void AbZipPrivate::flushAndClose() {

	qint64 retCode = centralDir->write( ioDevice );

	if ( retCode <= 0 )
		setError(AbZip::WriteFailed, QT_TR_NOOP("Error writing Central directory to the io device!") );

	// Do we need to truncate the file? Should never happen tho.
	// Commented out as is just too dangerous!!!
//    if ( retCode > 1 && !zipName.isEmpty() && ioDevice->pos() < ioDevice->size() )
//        qobject_cast<QFile*>(ioDevice)->resize( ioDevice->pos() );

	// Close io if we opened it
	if (!zipName.isEmpty())
		ioDevice->close();
};






QList<ZipFileInfo> AbZipPrivate::findFile( AbZip& zip, const QString& filename,
								  const QString& root, AbZip::ZipOptions options ) {

	QList<ZipFileInfo> list;

	if ( checkAutoOpenRead() )
	{
		if ( !nameFilters.isEmpty() || filename.contains('*') )
		{
			QStringList tmpNameFilter( nameFilters  );
			tmpNameFilter << filename;

			// We need to use the iterator since we are pattern matching the search
			ZipDirIterator it(zip, root, tmpNameFilter, options );
			while (it.hasNext())
			{
				it.next();
				list.append( it.fileInfo() );
			}
		}
		else
		{
			// Fast Find the file by direct path/name
			QString pathFile = filename;
			if ( !root.isEmpty() )
				pathFile = root + "/" + filename;

			CentralDirFileHeader* header =  centralDir->findFile( pathFile, options );
			if ( header )
			{
				list.append( ZipFileInfo(header) );
			}
		};


		// Sort?
		if ( list.size() > 1 &&
			 (options & (AbZip::SortByCompressedSize | AbZip::SortByUncompressedSize | AbZip::SortByType |
						AbZip::SortByTime | AbZip::SortByName ) ))
		{
			QList<ZipFileInfo> sortedList;
			sortInfoList( list,  sortedList, options );
			return sortedList;
		}
	};

	return list;
};



bool AbZipPrivate::extractAll(AbZip& zip, const QString& destPath, const QString& fromRoot, AbZip::ZipOptions options) {

	if ( !checkAutoOpenRead() )
		return false;

	if ( nameFilters.isEmpty() && fromRoot.isEmpty() )
	{
		// Fast extract ALL files from archive without the need to create a ZipIterator
		foreach( CentralDirFileHeader* header,  centralDir->entries )
		{
			if ( !extractFile( header, destPath, options ) &&
				 !options.testFlag( AbZip::ContinueOnError ) )
			{
				return false;
			}
		}
	}
	else
	{
		// We need to use the iterator since we are patern matching the extraction
		ZipDirIterator it(zip, fromRoot, nameFilters, options );
		while (it.hasNext())
		{
			it.next();
			if ( !extractFile( it.getCurrentHeader(), destPath, options ) &&
				 !options.testFlag( AbZip::ContinueOnError ) )
			{
				return false;
			}
		}
	};

	return true;
};


bool AbZipPrivate::extractFile(const QString& filename, const QString& destPath, AbZip::ZipOptions options) {

	if ( !checkAutoOpenRead() )
		return false;

	// Find the file
	CentralDirFileHeader* header =  centralDir->findFile( filename, options );
	if ( !header )
		return setError(AbZip::FileNotFound, QT_TR_NOOP("File '")+filename+ QT_TR_NOOP("' not found in zip file") );

	return extractFile( header, destPath, options);

};


bool AbZipPrivate::extractFile( CentralDirFileHeader* header, const QString& destPath, AbZip::ZipOptions options) {

	Q_ASSERT( header );

	QString destFilePath;

	if ( options.testFlag( AbZip::VerifyOnly ) == false )
	{
		// Construct the destination file
		if ( options.testFlag( AbZip::ExtractPaths ) )
			destFilePath = QDir::cleanPath( destPath + "/" + header->sFileName );
		else
			destFilePath = QDir::cleanPath( destPath + "/" + getFileName( header->sFileName ) );

		QFileInfo destFileInfo( destFilePath );

		// If the file exists and we can't overwrite, then return
		if ( !header->isDirectory() && destFileInfo.exists() && options & AbZip::DontOverwrite)
			return setError(AbZip::FileExists, QT_TR_NOOP("Destination file exists: ")+destFilePath );


		// Create the destination path
		QDir dir( header->isDirectory() ? destFilePath : destFileInfo.absolutePath() );
		if ( !dir.exists() )
		{
			if ( !dir.mkpath( dir.absolutePath() ) )
				return setError(AbZip::CreateDirError, QT_TR_NOOP("Failed to create folder: ")+dir.absolutePath() );
		};

		if ( header->isDirectory() )
			return true;        // nothing to do
	}
	else
	{
		qDebug() << "Checking validity of file:" << header->sFileName;
	};

	// seek to the start of the Local File Header
	if ( !ioDevice->seek( header->getRelativeOffset() ) )
		return setError(AbZip::SeekError,
						QString(QT_TR_NOOP("Failed to locate local file header in archive for '%1'")).arg(header->sFileName) );

	// Get the compressed size, this is how much to read from input
	qint64 compressedSizeToRead = header->getCompressedSize();

	// Read the local file header
	QScopedPointer<LocalFileHeader> localHeader( new LocalFileHeader );
	localHeader->read(ioDevice);

	// Validate local header with Central directory header
	if ( !localHeader->validate( header ) )
		return setError(AbZip::HeaderIntegrityError,
						QString(QT_TR_NOOP("Local header record does not match Central Directory record for '%1'")).arg(header->sFileName) );


	quint16 useCompressionMethod = header->compressionMethod;

	// If this file was encrypted, then test for valid password
	QScopedPointer<Encryption> encrypt;
	if ( localHeader->isEncrypted() )
	{
		Encryption* enc = Encryption::createEncryptor( useCompressionMethod );
		if ( enc )
			encrypt.reset( enc );
		else
			return setError(AbZip::InvalidEncryption, QString( QT_TR_NOOP("Encryption method %1 not supported to extract '%2'"))
							.arg(useCompressionMethod)
							.arg(header->sFileName) );

		quint16 check = localHeader->hasDataDescriptor() ? localHeader->lastModFileTime >> 8 : localHeader->crc_32 >> 16;

		if ( localHeader->compressionMethod == AES_METHOD )
		{
			// get the AES extra data
			quint8 strength = 0;
			if ( !localHeader->getWinZipAES( useCompressionMethod, strength ) )
				return setError(AbZip::InvalidAesData,
								QString(QT_TR_NOOP("Required AES Extra data not found to extract '%1'")).arg(header->sFileName) );
			check = strength;
		};

		if ( !encrypt->decryptHeader( ioDevice, password, check ) )
		{
			return setError(AbZip::InvalidPassword,
							QString(QT_TR_NOOP("Invalid password specified for extracting '%1'")).arg(header->sFileName) );
		};

		compressedSizeToRead -= encrypt->size();
	};


	// Open a temporary file to extract the file too
	QTemporaryFile tmpFile;

	if ( options.testFlag( AbZip::VerifyOnly ) == false )
	{
		if (!tmpFile.open())
			return  setError(AbZip::CreateFileError, QT_TR_NOOP("Failed to create temporary file for extraction"));
	};

	// Create the decompressor
	QScopedPointer<Compressor> compressor;

	Compressor* comp = Compressor::createDecompressor( useCompressionMethod, ioDevice, tmpFile.isOpen() ? &tmpFile : NULL );
	if ( comp )
		compressor.reset( comp );
	else
		return setError(AbZip::InvalidCompressor,
						QString(QT_TR_NOOP("Compressor not supported for extracting '%1'!")).arg(header->sFileName) );

	if ( compressor.data() && !compressor->decompressData( compressedSizeToRead, encrypt.data() ) )
		return setError(AbZip::ExtractFailed,
						QString(QT_TR_NOOP("Error extracting '%1' (%2)")).arg(header->sFileName).arg(compressor->errorString()) );

	tmpFile.close();

	if ( localHeader->isEncrypted() )
	{
		if ( encrypt->decryptFooter( ioDevice, 0 ) <= 0 )
		{
			return setError(AbZip::Corrupted, QString(QT_TR_NOOP("Corrupted data found for '%1' (AES authcode failed)")).arg(header->sFileName) );
		}
	};

	// Validate the CRC
	if ( comp->crc != header->crc_32 )
		return setError(AbZip::Corrupted, QString(QT_TR_NOOP("Corrupted data found for '%1' (crc check failed)")).arg(header->sFileName) );

	// Finally - finish by copying the temp file to required location and set its last modified date
	if ( options.testFlag( AbZip::VerifyOnly ) == false )
	{
		if ( QFile::exists( destFilePath ) && !options.testFlag( AbZip::DontOverwrite ) )
			QFile::remove(destFilePath);

		// Copy temp file to actual location
		if ( !tmpFile.copy( destFilePath ) )
			return setError(AbZip::CopyError,
							QString(QT_TR_NOOP("Failed to copy extracted file to '%1' (%2)")).arg( destFilePath ).arg(tmpFile.errorString()) );

		// Set the destination files date and time
		if ( !setFileDateTime(destFilePath, header->lastModDate() ) )
			setError(AbZip::SetModTimeFailed,
					 QString(QT_TR_NOOP("Unable to set last modified time for the extracted file '%1'")).arg( destFilePath ));

		filesExtracted++;
	};

	return true;
};

bool AbZipPrivate::addFile(const QString& file, const QString& root, AbZip::ZipOptions options, int level ) {

	if ( !checkAutoOpenWrite() )
		return false;

	// Check the input file exists!
	QFileInfo inFileInfo( file );
	if ( !inFileInfo.exists( ) )
		return setError(AbZip::FileNotFound, QT_TR_NOOP("File '")+file+ QT_TR_NOOP("' not found!") );

	// Construct the required archive path and file name
	QString archFileName;
	archFileName =  stripDriveLetter(root);
	if ( !archFileName.isEmpty() )
	{
		if ( !archFileName.endsWith('/'))
			archFileName += "/";
	};


	if ( options & AbZip::AddIgnorePaths )
	{
		archFileName += inFileInfo.fileName();
	}
	else if ( options & AbZip::AddAbsolutePaths )
	{
		archFileName += stripDriveLetter( inFileInfo.filePath() );
	}
	else // Default AbZip::AddRelativePaths
	{
		archFileName += inFileInfo.fileName();      // Just the filename if no root specified
	};


	return addFile( inFileInfo, archFileName, options, level );
};

bool AbZipPrivate::addFile(const QFileInfo& srcFileInfo, const QString& arcFile, AbZip::ZipOptions options, int level ) {

	// create a new CD file header and initialize it
	QScopedPointer<CentralDirFileHeader> newHeader(new CentralDirFileHeader);
	newHeader->initFromFile( srcFileInfo );


	// Always check if this file already exists
	CentralDirFileHeader* findHeader =  centralDir->findFile( arcFile, options );
	if ( findHeader )
	{
		// already added to archive.  Do we skip it?
		if ( options.testFlag( AbZip::SkipExistingFiles) )
			return true;

		// Do we skip it if it's newer? (default is to only add if newer)
		if ( options.testFlag( AbZip::AddReplaceFiles) == false )
		{
			// Only add if it's newer
			if ( !findHeader->isNewer( newHeader.data() ) )
				return true;    // it is newer so just return
		}
	};

	newHeader->setFileName( arcFile );

	// *** Handle Folders differently.  Just need to create a Local File Header with no data ***
	if ( srcFileInfo.isDir() )
	{
		// Create new local file header that will be destroyed when this function goes out of scope
		QScopedPointer<LocalFileHeader> localHeader(new LocalFileHeader);

		// init local header from CD file header
		localHeader->init( newHeader.data() );

		// Seek to the required point in the file where we can add new files
		ioDevice->seek( centralDir->offsetToStartOfCD() );

		// Save this point in the CD file header later
		newHeader->setSizes(0, 0, ioDevice->pos() );

		// Write the local file header
		localHeader->write( ioDevice );

		centralDir->setOffsetToStartOfCD( ioDevice->pos() );
		centralDir->setModified();

		// Add the new CD File Header to CD list and return
		centralDir->entries.append( newHeader.take() );
		return true;
	};

	// It's a file so add the file data.......

	if ( ioDevice->isSequential() )
		newHeader->generalFlag |= ZIP_WRITE_DATA_DESCRIPTOR;

	if ( !password.isEmpty() )
		newHeader->generalFlag |= ZIP_IS_ENCRYPTED;

	// Open the input file
	QFile inFile(srcFileInfo.absoluteFilePath());
	if (!inFile.open(QIODevice::ReadOnly))
		return setError(AbZip::OpenFileError,
						QString(QT_TR_NOOP("Failed to open file '%1' (%2)")).arg(srcFileInfo.absoluteFilePath()).arg(inFile.errorString()) );

	// Determine the method ( 0 = Store or 8 = Deflate).  If level = 0 then always use store
	if ( level != 0 )
		newHeader->compressionMethod = getBestCompressionMethod( inFile, options );
	else
		newHeader->compressionMethod = methodStore;

#ifdef USE_BZIP2
	if ( options.testFlag(AbZip::useBZip2Compression))
		newHeader->versionNeeded = 45;
#endif
#ifdef USE_LZMA
	if ( options.testFlag(AbZip::useLzmaCompression))
	{
		newHeader->versionNeeded = 63;
		//newHeader->generalFlag |= 2;
	}
#endif

	if (newHeader->compressionMethod == methodDeflate)
	{
		// As per PKWARE Spec 4.4.4
		if ( level == 8 || level == 9 || level == -1 )
		  newHeader->generalFlag |= 2;
		else if (level == 2)
		  newHeader->generalFlag |= 4;
		else if (level == 1)
		  newHeader->generalFlag |= 6;
	};

	if (newHeader->compressionMethod == methodStore && ioDevice->isSequential() == false )
		newHeader->versionNeeded = 10; // version 1.0 needed only

	// Create new local file header that will be destroyed when this function goes out of scope
	QScopedPointer<LocalFileHeader> localHeader(new LocalFileHeader);

	// init local header from CD file header
	localHeader->init( newHeader.data() );

	// Seek to the required point in the file where we can add new files
	ioDevice->seek( centralDir->offsetToStartOfCD() );

	// Save this point in the CD file header later
	qint64 relativeOffset = ioDevice->pos();

	// Save the compression method here as WinZip AES changes it to 99!
	quint32 useCompressionMethod = newHeader->compressionMethod;
#ifdef USE_AES
	if (options & AbZip::useAESEncryption )
	{
		// Create the Extra data entrys for WinZip AES
		localHeader->setWinZipAES();
		newHeader->setWinZipAES();
	}
#endif

	// Write the local file header
	localHeader->write( ioDevice );

	// Any encryption ?
	QScopedPointer<Encryption> encrypt;
	if ( newHeader->isEncrypted() )
	{
		Encryption* enc = Encryption::createEncryptor( (options & AbZip::useAESEncryption ) ? encAES : encCRC );
		if ( enc )
			encrypt.reset( enc );
		else
			return setError(AbZip::InvalidEncryption, QT_TR_NOOP("Encryption method not supported!") );

		quint16 crc = localHeader->lastModFileTime >> 8; // the default for sequencial io

		// If not sequencial then we need to get the crc of the file
		if ( !ioDevice->isSequential() )
		{
			// Use the StoreCompressor to simply read and get the CRC
			qint64 curPos = inFile.pos();
			StoreCompressor crcCheck( &inFile );
			crcCheck.compressData();
			inFile.seek( curPos );

			// We should have the crc of the input now!
			crc = crcCheck.crc >> 16;   // hi word
		};

		encrypt->encryptHeader( ioDevice, password, crc );
	};


	// Create the Compressor
	QScopedPointer<Compressor> compressor;

	Compressor* comp = Compressor::createCompressor( useCompressionMethod, level, &inFile, ioDevice );
	if ( comp )
		compressor.reset( comp );
	else
		return setError(AbZip::InvalidCompressor,
						QString(QT_TR_NOOP("Compressor not supported for extracting '%1'!")).arg(newHeader->sFileName) );

	// Compress the data
	if ( !compressor->compressData( encrypt.data() ) )
		return setError(AbZip::AddFileFailed,
						QString(QT_TR_NOOP("Error compressing '%1' (%2)")).arg(newHeader->sFileName).arg(compressor->errorString()) );

	inFile.close();

	// Add the size of the encryption header to overall compressed size
	if ( newHeader->isEncrypted() )
	{
		encrypt->encryptFooter( ioDevice );  // if one is needed (e.g. WinZip AES)

		comp->compressedSize += encrypt->size();
	};

	// Set the crc and sizes in both local and central file headers (this handles saving to 64 bit)
	localHeader->crc_32 = compressor->crc;
	localHeader->setSizes(compressor->uncompressedSize, compressor->compressedSize, relativeOffset );

	newHeader->crc_32 = compressor->crc;
	newHeader->setSizes(compressor->uncompressedSize, compressor->compressedSize, relativeOffset );
	newHeader->internalFileAttr = compressor->isAscii ? 0x01 : 0;


	if ( ioDevice->isSequential() )
	{
		// Write the data descriptor here as we can't seek back
		localHeader->writeDataDescriptor( ioDevice );
	}
	else
	{
		qint64 curPos = ioDevice->pos();

		// Go back and rewrite the local header to update the crc, sizes and encryption etc.
		ioDevice->seek( relativeOffset );

		localHeader->write( ioDevice );

		// return to where we were
		ioDevice->seek( curPos );
	};

	// save the point where we can add more files OR close and add the CD
	centralDir->setOffsetToStartOfCD( ioDevice->pos() );

	// Add the new CD File Header to CD list
	centralDir->entries.append( newHeader.take() );

	centralDir->setModified();

	if ( findHeader )
	{
		// we need to remove the old file
		deleteFile( findHeader );
	};

	filesAdded++;

	return true;
};


bool AbZipPrivate::addDirectory(const QString& srcPath, const QString& root, AbZip::ZipOptions options, int level ) {

	if ( !checkAutoOpenWrite() )
		return false;


	QDir srcDir(srcPath);
	if (!srcDir.exists())
		return setError( AbZip::FileNotFound, QT_TR_NOOP("Folder not found") );


	QDir::Filters filters = QDir::Dirs | QDir::Files| QDir::NoDotAndDotDot | QDir::NoSymLinks;
	if ( options.testFlag( AbZip::IgnoreFolders ) )
		filters &= ~QDir::Dirs;
	if ( options.testFlag( AbZip::IgnoreFiles ) )
		filters &= ~QDir::Files;

	srcDir.setFilter( filters );
	srcDir.setNameFilters( nameFilters );

	// Create another QDir but back one level. This become our relative root path
	QDir dirRel( root.isEmpty() ? srcDir : stripDriveLetter(root) );

	// I want the default to always be AddRelativePaths - hence I do it like this!
	if ( root.isEmpty() && !options.testFlag( AbZip::AddAbsolutePaths ) &&
						   !options.testFlag( AbZip::AddIgnorePaths ) )
		dirRel.cdUp();

	QString arcFilePath;

	// Iterate over all files in folder and optional sub folders
	QDirIterator it( srcDir, options.testFlag( AbZip::Recursive ) ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
	while(it.hasNext())
	{
		it.next();

		if ( options.testFlag( AbZip::AddAbsolutePaths ))
			arcFilePath = stripDriveLetter( dirRel.absoluteFilePath( it.filePath()) );
		else if ( options & AbZip::AddIgnorePaths )
			arcFilePath = it.fileName();
		else // AddRelativePaths
			arcFilePath = stripDriveLetter( dirRel.relativeFilePath( it.filePath() ) );

		if ( !addFile( it.fileInfo(), arcFilePath, options, level ) &&
			 !options.testFlag( AbZip::ContinueOnError ) )
		{
			return false;
		}
	};

	return true;
};

quint32 AbZipPrivate::getBestCompressionMethod( QFile& file, AbZip::ZipOptions options) {

#ifdef USE_BZIP2
	if ( options.testFlag(AbZip::useBZip2Compression))
		return methodBzip2;     // User opted for BZip2 as default
#endif
#ifdef USE_LZMA
	if ( options.testFlag(AbZip::useLzmaCompression))
		return methodLzma;     // User opted for LZMA as default
#endif

	// Don't compress any files too small!
	if ( file.size() < 100 )
		return methodStore;
	else if ( !options.testFlag(AbZip::IgnoreStoreOnlyChecks))
	{
		// Don't compress compressed files
		QString filename = file.fileName();
		foreach( const QString& ext, storeTheseTypes  )
		{
			if ( filename.contains( ext, Qt::CaseInsensitive ) )
				return methodStore;
		};

		// Take a peek at the file header for known compressed file signatures (lots of MS Office files use zips now)
		// We should add more compressed file signatures here!!
		QByteArray ba = file.peek( 50 );
		if ( ba.contains( "\x50\x4b\x03\x04" ) )    // zip local header signature
			return methodStore;
	}
	return methodDeflate;   // default
};


bool AbZipPrivate::deleteFile(AbZip& zip, const QString& filename, AbZip::ZipOptions options) {

	if ( !checkAutoOpenWrite() )
		return false;

	if ( !nameFilters.isEmpty() || filename.contains('*') )
	{
		QStringList tmpNameFilter( nameFilters  );
		if ( filename.contains('*') )
			tmpNameFilter << filename;

		ZipDirIterator it(zip, QString(), tmpNameFilter, options );
		while (it.hasNext())
		{
			it.next();
			if ( !deleteFile( it.getCurrentHeader() ) &&
				 !options.testFlag( AbZip::ContinueOnError ) )
			{
				return false;
			}
		}
		return true;
	};

	// Find the file by direct path/name
	CentralDirFileHeader* header =  centralDir->findFile( filename, options );
	if ( !header )
		return setError(AbZip::FileNotFound,
						QString(QT_TR_NOOP("File '%1' not found in archive file")).arg(filename) );

	return deleteFile( header );
};

bool AbZipPrivate::deleteFile(CentralDirFileHeader* header, bool dontLogIt ) {

	int index = centralDir->entries.indexOf( header );
	if ( index >= 0 )
	{
		centralDir->entries.removeAt( index );
		centralDir->setModified();
	};

	if ( !dontLogIt )
		filesDeleted++;

	// Add to the deletedEntries list
	deletedEntries.append( header );
	return true;
};

bool AbZipPrivate::renameFile( const QString& oldFilename, const QString& newFilename, AbZip::ZipOptions options) {

	if ( !checkAutoOpenWrite() )
		return false;

	// Find the file
	CentralDirFileHeader* header =  centralDir->findFile( oldFilename, options );
	if ( !header )
		return setError(AbZip::FileNotFound, QT_TR_NOOP("File '")+oldFilename+ QT_TR_NOOP("' not found in zip file") );

	// The new filename must not exist
	if ( centralDir->findFile( newFilename, options ) )
		return setError(AbZip::FileExists, QT_TR_NOOP("File '")+newFilename+ QT_TR_NOOP("' already exists in zip file!") );

	// seek to the start of the Local File Header
	if ( !ioDevice->seek( header->getRelativeOffset() ) )
		return setError(AbZip::SeekError,
						QString(QT_TR_NOOP("Failed to locate local file header in archive for '%1'")).arg(header->sFileName) );

	// Read the local file header
	QScopedPointer<LocalFileHeader> localHeader( new LocalFileHeader );
	localHeader->read(ioDevice);

	// Validate local header with Central directory header
	if ( !localHeader->validate( header ) )
		return setError(AbZip::HeaderIntegrityError,
						QString(QT_TR_NOOP("Local header record does not match Central Directory record for '%1'")).arg(header->sFileName) );

	// Set new filename in localHeader
	localHeader->setFileName( newFilename );

	// Are we lucky enough to have the same length filenames?
	if ( oldFilename.size() == newFilename.size() )
	{
		// Seek back to start of localHeader and write with update filename
		ioDevice->seek( header->getRelativeOffset() );
		localHeader->write(ioDevice);

		// Update the filename in the Central Directory
		header->setFileName( newFilename );
		centralDir->setModified();

		// job done!
		return true;
	};

	// Ok so we need to re-add this file and delete the old one.  I don't see any other way!

	// First save the current file pos so we can copy the file data from here and add as a new file
	qint64 readPos = ioDevice->pos();

	// Seek to the required point in the file where we can add new files
	ioDevice->seek( centralDir->offsetToStartOfCD() );

	// Save this point in the new CD file header later
	qint64 relativeOffset = ioDevice->pos();

	// Write the local file header
	localHeader->write( ioDevice );

	qint64 compressedSizeToRead = header->getCompressedSize();

	// StoreCompressor is used simply to do the copying within the device
	StoreCompressor copy( ioDevice, ioDevice );
	if ( !copy.copyDataInSameFile( readPos, ioDevice->pos(), compressedSizeToRead ) )
	{
		centralDir->setModified();  // recover from an overwritten CD
		return setError(AbZip::WriteFailed,
						QString(QT_TR_NOOP("Error copying data as part of the renaming process '%1'")).arg(header->sFileName) );
	};

	if ( ioDevice->isSequential() )
	{
		// Write the data descriptor here as we can't seek back
		localHeader->writeDataDescriptor( ioDevice );
	};

	// save the point where we can add more files OR close and add the CD
	centralDir->setOffsetToStartOfCD( ioDevice->pos() );

	// Make a copy of the old header
	CentralDirFileHeader* newHeader =  new CentralDirFileHeader( *header );
	newHeader->setFileName( newFilename );
	newHeader->relativeOffset = relativeOffset;

	// Add the new CD File Header to CD list
	centralDir->entries.append( newHeader );

	// Delete the old file
	deleteFile( header, true );

	centralDir->setModified();

	return true;
};

bool sortHeaderByOffset( CentralDirFileHeader* h1, CentralDirFileHeader* h2) {

	return h1->getRelativeOffset() < h2->getRelativeOffset();
};

bool AbZipPrivate::commitDeletedFiles() {

	if ( deletedEntries.size() == 0 )
		return false;

	// Open a temporary zip file
	QTemporaryFile tmpFile;
	if (!tmpFile.open())
		return setError(AbZip::CreateFileError, QT_TR_NOOP("Failed to create temporary archive to delete files"));

	// Since we adjust the relativeOffset in the central dir during this commit process, we need to make a copy of all
	// CentralDirFileHeader relativeOffset's so if something goes wrong we can put them all back!
	QMap<CentralDirFileHeader*, qint64> savedOffsets;
	for( int i=0; i < centralDir->entries.size(); i++ )
	{
		CentralDirFileHeader* header =  centralDir->entries.at(i);
		savedOffsets.insert( header, header->getRelativeOffset() );
	};

	bool ok = writeToTempFile( &tmpFile );
	if ( !ok )
	{
		// Error!!!  Put back the offsets
		for( QMap<CentralDirFileHeader*,qint64>::iterator it = savedOffsets.begin(); it != savedOffsets.end(); )
		{
			CentralDirFileHeader* header = it.key();
			header->setSizes( header->getUncompressedSize(), header->getCompressedSize(), it.value() );
		}
	};

	flushAndClose();    // Close the current ioDevice

	// Clean up the deleted entries list
	qDeleteAll( deletedEntries );
	deletedEntries.clear();

	if ( ok )
	{
		// Copy the temp file back to original
		QFile::remove(zipName);

		// close the temp file before we copy it
		tmpFile.close();

		// Copy temp file back to this zip
		if ( !tmpFile.copy( zipName ) )
			return setError(AbZip::CopyError, QString(QT_TR_NOOP("Failed to copy temp archive file back to '%1' (%2)")).arg( zipName ).arg(tmpFile.errorString()) );
	};

	return ok;
};

bool AbZipPrivate::writeToTempFile( QIODevice* ioTemp ) {

	// StoreCompressor is used simply to do the copying from one io device to another
	StoreCompressor copy( ioDevice, ioTemp );

	// First sort the list by offset in the file
	qSort( deletedEntries.begin(), deletedEntries.end(), sortHeaderByOffset );

	qint64 readStartPos = 0;
	ioDevice->seek(0);  // make sure we are at the start of the file
	qint64 relativeOffset;

	// Go through each header in order of offset
	foreach( CentralDirFileHeader* header, deletedEntries )
	{
		// seek to the start of the Local File Header
		relativeOffset = header->getRelativeOffset();
		if ( !ioDevice->seek( relativeOffset ) )
			return setError(AbZip::SeekError,
							QString(QT_TR_NOOP("Failed to locate local file header in archive for '%1'")).arg(header->sFileName) );

		// Get the compressed size, this is how much to read for this file
		qint64 sizeToDelete = header->getCompressedSize();

		// Read the local file header just to check it's the correct file to delete
		QScopedPointer<LocalFileHeader> localHeader( new LocalFileHeader );
		localHeader->read(ioDevice);

		// Validate this is the correct record
		if ( !header->validate( localHeader.data() ) )
			return setError(AbZip::HeaderIntegrityError,
							QString(QT_TR_NOOP("Local header record does not match Central Directory record for '%1'")).arg(header->sFileName) );

		// Add the size of the local header to the delete size
		sizeToDelete += localHeader->size();

		// if has a data description, then add this size on as well
		// WARNING: I don't know the true size of the data descriptor as there may or may not be a signature!!!
		// I will assume not.  In that way I don't corrupt the next file but just leave 4 bytes hanging around in the file
		// TODO: A better way is to write the actual files found in the current CD 1 by 1.  This would also clean the zip of problems.
		if ( header->hasDataDescriptor() )
			sizeToDelete += (DATA_DESCRIPTOR_SIZE-4);


		// OK - lets start copying data over to the temp file
		ioDevice->seek( readStartPos );

		if ( !copy.copyData( relativeOffset-readStartPos ) )
			return setError(AbZip::DeleteError,
							QT_TR_NOOP("Delete File: Failed to copy existing archive data to temp file"));

		// Jump to the end of this files data in the zip
		ioDevice->seek( relativeOffset + sizeToDelete );

		// Adjust all offsets past this one in the central directory to reflect deleted data size
		centralDir->adjustOffsets( relativeOffset, sizeToDelete );

		// keep going
		readStartPos = ioDevice->pos();
	};


	// Copy all data from this point up to the start of the Central Dir
	relativeOffset = centralDir->offsetToStartOfCD();

	if ( !copy.copyData( relativeOffset-readStartPos ) )
		return setError(AbZip::DeleteError, QT_TR_NOOP("Delete File: Failed to copy existing archive data to temp file"));

	// Set the new Offset to CD
	centralDir->setOffsetToStartOfCD( ioTemp->pos() );

	// Write the new CD to tmp file
	if ( !centralDir->write( ioTemp ))
		return setError(AbZip::WriteFailed, QT_TR_NOOP("Error writing Central directory to the io device!") );

	// All gone well
	return true;
};


// Simple integrity checking.  Does not CRC check file data at this point
bool AbZipPrivate::checkIntegrity() {

	if ( !checkAutoOpenRead() )
		return false;

	QScopedPointer<LocalFileHeader> localHeader( new LocalFileHeader );
	foreach( CentralDirFileHeader* header,  centralDir->entries )
	{
		extractFile( header, "", AbZip::VerifyOnly );
	};

	return errorMsg.isEmpty();
};

static qint64 locateSignature( quint32 signature, QIODevice* ioDevice  ) {

	QByteArray baSig = QByteArray::fromRawData((const char*) &signature, sizeof(quint32)) ;

	qint64 fileSize = ioDevice->size();
	qint64 startPoint = ioDevice->pos();

	int readSize = 0x400;     // max read size

	qint64 foundPos = -1;

	QByteArray ba;

	qint64 sizeRead = 0;
	while ( sizeRead < fileSize )
	{
		readSize = qMin( (qint64)readSize, fileSize-sizeRead);

		// Read a block from file
		ba.clear();
		ba = ioDevice->read( readSize );
		if ( ba.size() != readSize )
		{
			// Error reading from file
			return -1;
		};


		// Look for signature
		for (int i = 0; i < (int)readSize-3; i++ )
		{
			if ( ba[i]==baSig[0] && ba[i+1]==baSig[1] &&
				 ba[i+2]==baSig[2] && ba[i+3]==baSig[3] )
			{
				foundPos = startPoint + sizeRead + i;
				break;
			}
		}
		sizeRead += readSize;

		if ( foundPos >= 0 )
			break;
	};

	return foundPos;
};

// If silent is true, then we are attempting an auto recovery
bool AbZipPrivate::repairArchive( ) {

	// Check if it is bad
	if( checkIntegrity() )
	{
		setError(AbZip::Ok, QT_TR_NOOP("The archive appears to be OK!  Repair not required.") );
		return true;
	};

	if ( !zipName.isEmpty() )
	{
		// Close the archive
		if ( isOpen())
			close();

		// Create a backup of original file
		QFileInfo fileInfo( zipName );
		QString backupFilename = fileInfo.absolutePath() + "/" + fileInfo.baseName() + QT_TR_NOOP(" (bad).") + fileInfo.completeSuffix();

		if ( QFile::exists(backupFilename) )
			QFile::remove(backupFilename);
		if ( !QFile::copy(zipName, backupFilename) )
			setError(AbZip::CopyError, QT_TR_NOOP("Failed to create back of current archive") );

		// re-open archive for reading/writing
		ioDevice = new QFile(zipName);
		if (!ioDevice->open(QIODevice::ReadWrite))
			return setError(AbZip::OpenFileError, QT_TR_NOOP("Cannot open device for reading: ")+ioDevice->errorString() );
	};


	if ( !rebuildCentralDirectory() )
	{
		return setError(AbZip::InvalidArchive, QT_TR_NOOP("This does not appear to be a valid Zip file!") );
	};

	setError(AbZip::Ok, QString( QT_TR_NOOP("repairArchive recovered %1 files") ).arg(centralDir->entries.size()) );
	return true;
};

bool AbZipPrivate::rebuildCentralDirectory( ) {

	ioDevice->seek( centralDir->bytesBeforeZip );

	QScopedPointer<LocalFileHeader> localHeader( new LocalFileHeader );

	// Start reading LocalHeaders
	qint64 localHeaderPos = -1;
	while( (localHeaderPos = locateSignature( LOCAL_FILE_HEADER_SIGNATURE, ioDevice )) >= 0 )
	{
		// Found a local header
		ioDevice->seek( localHeaderPos );
		// Read it
		if ( localHeader->read(ioDevice) )
		{
			// Jump to end of local file data
			if ( localHeader->hasDataDescriptor() )
			{
				// We don't know the crc or compressed data size yet!
				// WARNING: this assumes we have a data descriptor signature.  If not then.......!!!!!!

				qint64 curPos = ioDevice->pos();    // save current file pos
				qint64 nextLocalHeaderPos = locateSignature( LOCAL_FILE_HEADER_SIGNATURE, ioDevice );
				ioDevice->seek( curPos );

				// Search for DataDescriptor header
				qint64 dataDescriptorPos = locateSignature( DATA_DESCRIPTOR_SIGNATURE, ioDevice );
				if ( dataDescriptorPos > 0 && (nextLocalHeaderPos == -1 || dataDescriptorPos < nextLocalHeaderPos ) )
				{
					// Found a valid data descriptor for this local file
					ioDevice->seek( dataDescriptorPos );
					localHeader->readDataDescriptor( ioDevice );
				}
			}
			else
			{
				// We know the compressed size so seek to end of this local file data
				ioDevice->seek( localHeaderPos + localHeader->getCompressedSize() );
			};

			// Create the new CD header
			CentralDirFileHeader* cdHeader = new CentralDirFileHeader();

			// copy all info found in local header into this new CD header
			cdHeader->initFromLocalHeader( *(localHeader.data()) );
			cdHeader->setSizes( localHeader->getUncompressedSize(), localHeader->getCompressedSize(), localHeaderPos );

			// Add the new CD File Header to CD list
			centralDir->entries.append( cdHeader );
		}
	};

	// Did we find any LocalHeaders?
	if ( centralDir->entries.size() > 0 )
	{
		centralDir->setOffsetToStartOfCD( ioDevice->pos() );
		centralDir->setModified();
		centralDir->isValid = true;
		return true;
	};

	return false;
};



bool AbZipPrivate::setError( int err, QString msg ) {

	errorCode = err;   // Stores only the last error

	// Store multiple error messages separated by new line
	if ( !errorMsg.isEmpty() )
		errorMsg += "\n";
	errorMsg += msg;

	errorCount++;

	qDebug() << msg;

	return false;
};

ZipInfo AbZipPrivate::getInfo() {

	ZipInfo info;

	bool autoClose = false;
	if ( !isOpen() )
	{
		// If not open then auto open for reading
		if ( !open(AbZip::modeOpenRead ) )
			return info;

		autoClose = true;
	};

	info = centralDir->getInfo( );
	if ( ioDevice )
		info.fileSize = ioDevice->size();

	info.filesAdded = filesAdded;
	info.filesExtracted = filesExtracted;
	info.filesDeleted = filesDeleted;
	info.errorCount = errorCount;

	if ( autoClose )
		close();

	return info;
};


//////////////////////////////////////////////////////////////////////////////////////////////

AbZip::AbZip(QObject *parent) : QObject(parent),
	d_ptr(new AbZipPrivate()) {

};

AbZip::AbZip(const QString& zipName, QObject *parent) :  QObject(parent),
	d_ptr(new AbZipPrivate(zipName)) {

};

AbZip::AbZip(QIODevice *ioDevice, QObject *parent) :  QObject(parent),
	d_ptr(new AbZipPrivate(ioDevice)) {

};

AbZip::~AbZip() {

	delete d_ptr;
};


bool AbZip::open( Mode mode, AbZip::ZipOptions options ) {

	return d_ptr->open( mode, options );
};

void AbZip::close() {

	d_ptr->close();
};

bool AbZip::isOpen() {

	return d_ptr->isOpen();
};


void AbZip::setZipFileName( const QString& name ) {

	d_ptr->zipName = name;
};


void AbZip::setPassword( const QString& pw ) {

	d_ptr->password = pw;
};

void AbZip::clearPassword( ) {

	d_ptr->password.clear();
};

int AbZip::errorCode() const
{
	return d_ptr->errorCode;
};

int AbZip::errorCount() const
{
	return d_ptr->errorCount;
};

const QString& AbZip::errorString() const
{
	return d_ptr->errorMsg;
};

void AbZip::clearErrors() {

	d_ptr->errorCount = 0;
	d_ptr->errorCode = 0;
	d_ptr->errorMsg.clear();
};



bool AbZip::setComment(const QString& comment) {

	if ( d_ptr->isOpen() )
	{
		d_ptr->centralDir->endOfCentralDir.setComment( comment );
		return true;
	}
	return false;
};

QString AbZip::getComment()const
{
	return d_ptr->centralDir->endOfCentralDir.getComment();
};

ZipInfo AbZip::getInfo()const
{
	return d_ptr->getInfo( );
};

void AbZip::setNameFilters( const QStringList& nameFilters ) {

	d_ptr->nameFilters = nameFilters;
};

void AbZip::clearNameFilters( ) {

	d_ptr->nameFilters.clear();
};



bool AbZip::addFile(const QString& file, const QString& root, AbZip::ZipOptions options, int level ) {

	return d_ptr->addFile(file, root, options, level );
};

bool AbZip::addFile(const QString& file, AbZip::ZipOptions options, int level ) {

	return d_ptr->addFile(file, QString(), options, level );
};


bool AbZip::writeFile(const QString& file, const QByteArray& data, AbZip::ZipOptions options, int level ) {

	/* Write data to a temporary file */
	QFile f( "/tmp/" + baseName( file ) );
	f.open( QFile::WriteOnly );
	f.write( data );
	f.close();

	/* Add the temporary file */
	bool ret = d_ptr->addFile( "/tmp/" + baseName( file ), QString( dirName( file ) ), options, level );

	/* Remove temporary file */
	f.remove();

	return ret;
};

bool AbZip::addDirectory(const QString& srcPath, const QString& root, AbZip::ZipOptions options, int level ) {

	return d_ptr->addDirectory( srcPath, root, options, level );
};

bool AbZip::addDirectory(const QString& srcPath, AbZip::ZipOptions options, int level ) {

	return d_ptr->addDirectory( srcPath, QString(), options, level );
};


bool AbZip::extractFile(const QString& filename, const QString& destPath, AbZip::ZipOptions options) {

	return d_ptr->extractFile(filename, destPath, options);
};


bool AbZip::extractFiles(const QStringList& listFiles, const QString& destPath, ZipOptions options) {

	qint32 count = 0;
	foreach( const QString& filename, listFiles)
	{
		if ( d_ptr->extractFile( filename, destPath, options ) )
			count++;
		else if ( !options.testFlag( AbZip::ContinueOnError ) )
			return false;
	};

	return (count == listFiles.count()) ? true : false;
};


bool AbZip::deleteFile(const QString& filename, AbZip::ZipOptions options) {

	return d_ptr->deleteFile(*this, filename, options);

};

bool AbZip::renameFile(const QString& oldFilename, const QString& newFilename, AbZip::ZipOptions options) {

	return d_ptr->renameFile( oldFilename, newFilename, options);

};

bool AbZip::extractAll(const QString& path, AbZip::ZipOptions options) {

	return d_ptr->extractAll(*this, path, QString(), options);
};

bool AbZip::extractAll(const QString& path, const QString& fromRoot, AbZip::ZipOptions options) {

	return d_ptr->extractAll(*this, path, fromRoot, options);
};


QList<ZipFileInfo> AbZip::findFile( const QString& filename, ZipOptions options  ) {

	return d_ptr->findFile(*this, filename, QString(), options);
};

QList<ZipFileInfo> AbZip::findFile( const QString& filename, const QString& root, ZipOptions options ) {

	return d_ptr->findFile(*this, filename, root, options);
};

bool AbZip::checkIntegrity() {

	return d_ptr->checkIntegrity();

};

bool AbZip::repairArchive() {

	return d_ptr->repairArchive();

};

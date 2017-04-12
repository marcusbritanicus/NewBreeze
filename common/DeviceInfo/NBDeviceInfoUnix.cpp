/*
	*
	* NBDeviceInfoUnix.cpp - NBDeviceInfoPrivate Class for Unix
	*
*/

#include "common.hpp"
#include "NBDeviceInfo_p.hpp"

NBDeviceInfoPrivate::NBDeviceInfoPrivate() {

	/* Invalid Device Info */
	mIsValid = false;

	/* Device block path */
	device = "";

	/* File System Type */
	fileSystemType = "";

	/* Display Label */
	label = "";

	/* Mounted path */
	mountPoint = "";

	/* Device size, used size and available size */
	bytesTotal = 1;
	bytesUsed = 0;
	bytesAvailable = 0;

	/* FS block size */
	blockSize = 1;

	/* Is the FS mounted readonly */
	readOnly = true;
};

NBDeviceInfoPrivate::NBDeviceInfoPrivate( QString dev, QString mountPt, QString fs ) : QSharedData() {

	/* Check if the path provided and hence the NBDeviceInfo object is valid or not */
	mIsValid = true;

	mountPoint = mountPt.endsWith( "/" ) ? mountPt : mountPt + "/" ;
	device = dev;
	fileSystemType = fs;

	/* Get device label */
	getDeviceLabel();

	/* Read the Storage Information */
	retrieveVolumeInfo();
};

bool NBDeviceInfoPrivate::isValid() {

	return mIsValid;
};

void NBDeviceInfoPrivate::getDeviceLabel() {

	if ( mountPoint.compare( "/" ) == 0 )
		label = QString( "FileSystem" );

    static const QString pathDiskByLabel( "/dev/disk/by-label/" );

    struct dirent **fileList;
    int numFiles = scandir( pathDiskByLabel.toLocal8Bit().constData(), &fileList, NULL, NULL );

    for( int i = 0; i < numFiles; i++ ) {
		QString entryName( fileList[ i ]->d_name );
		if ( QString( entryName ).compare( "." ) == 0 )
			continue;

		if ( QString( entryName ).compare( ".." ) == 0 )
			continue;

		if ( not isLink( pathDiskByLabel + entryName ) )
			continue;

		if ( readLink( pathDiskByLabel + entryName ).compare( device ) == 0 ) {
			label = QString( entryName ).replace( "\\x20", " " );
			return;
		}
	};

    label = baseName( mountPoint );
};

void NBDeviceInfoPrivate::retrieveVolumeInfo() {

	if ( not mIsValid )
		return;

	struct statvfs statfs_buf;
	if ( statvfs( mountPoint.toLocal8Bit().constData(), &statfs_buf ) != 0 ) {
		qDebug() << qPrintable( QString( "[Error %1]: %2: %3" ).arg( errno ).arg( strerror( errno ) ).arg( mountPoint ) );
		mIsValid = false;
		bytesTotal = 1;
		bytesUsed = 0;
		bytesAvailable = 1;
		blockSize = 1;
		return;
	}

	/* FS Block Size */
	blockSize = statfs_buf.f_bsize;

	/* Size info */
	bytesTotal = statfs_buf.f_blocks * blockSize;
	bytesUsed = ( statfs_buf.f_blocks - statfs_buf.f_bfree ) * blockSize;
	bytesAvailable = statfs_buf.f_bavail * blockSize;

	/* Read Only mount? */
	readOnly = ( statfs_buf.f_flag & ST_RDONLY ) != 0;
};

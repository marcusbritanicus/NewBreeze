/*
	*
	* NBDeviceInfo.cpp - NBDeviceInfo Class
	*
*/

#include "NBDeviceInfo.hpp"
#include "NBDeviceInfo_p.hpp"

NBDeviceInfo::NBDeviceInfo() {

	d = new NBDeviceInfoPrivate();
};

NBDeviceInfo::NBDeviceInfo( const QString path ) {

	d = NBDeviceInfo( NBDeviceManager::deviceInfoForPath( path ) ).d;
};

NBDeviceInfo::NBDeviceInfo( const NBDeviceInfo &info ) {

	d = info.d;
};

NBDeviceInfo::NBDeviceInfo( NBDeviceInfoPrivate *infoPriv ) {

	d = QExplicitlySharedDataPointer<NBDeviceInfoPrivate>( infoPriv );
};

NBDeviceInfo::~NBDeviceInfo() {

};

NBDeviceInfo &NBDeviceInfo::operator=(const NBDeviceInfo &other) {

    d = other.d;
    return *this;
};

QString NBDeviceInfo::mountPoint() const {

	return d->mountPoint;
};

QString NBDeviceInfo::device() const {

	return d->device;
};

QString NBDeviceInfo::fileSystemType() const {

	return d->fileSystemType;
};

QString NBDeviceInfo::deviceType() const {

	return d->deviceType;
};

QString NBDeviceInfo::displayName() const {

	return d->label;
};

qint64 NBDeviceInfo::bytesTotal() const {

	return d->bytesTotal;
};

qint64 NBDeviceInfo::bytesUsed() const {

	return d->bytesUsed;
};

qint64 NBDeviceInfo::bytesAvailable() const {

	return d->bytesAvailable;
};

int NBDeviceInfo::blockSize() const {

	return d->blockSize;
};

bool NBDeviceInfo::isReadOnly() const {

	return d->readOnly;
};

bool NBDeviceInfo::isValid() const {

	return d->mIsValid;
};

bool NBDeviceManager::init = false;
QHash<QString, NBDeviceInfo> NBDeviceManager::devicesList = QHash<QString, NBDeviceInfo>();

QList<NBDeviceInfo> NBDeviceManager::allMounts() {

	pollDevices();

	return devicesList.values();
};

QList<NBDeviceInfo> NBDeviceManager::allDrives() {

	pollDevices();

	QList<NBDeviceInfo> devList;
	Q_FOREACH( NBDeviceInfo info, devicesList.values() ){
		if ( info.device().startsWith( "/dev/" ) )
			devList << info;
	}

	return devList;
};

QList<NBDeviceInfo> NBDeviceManager::allVirtualMounts() {

	pollDevices();

	QList<NBDeviceInfo> devList;
	Q_FOREACH( NBDeviceInfo info, devicesList.values() ) {
		if ( info.device().startsWith( "/dev/" ) )
			continue;

		if ( info.mountPoint().startsWith( "/run/" ) )
			continue;

		if ( info.mountPoint().startsWith( "/proc/" ) )
			continue;

		if ( info.mountPoint().startsWith( "/sys/" ) )
			continue;

		devList << info;
	}

	return devList;
};

QList<NBDeviceInfo> NBDeviceManager::allEncryptedMounts() {

	pollDevices();

	QList<NBDeviceInfo> devList;
	Q_FOREACH( NBDeviceInfo info, devicesList.values() ) {
		if ( info.fileSystemType().contains( "encfs" ) )
			devList << info;

		else if ( info.fileSystemType().contains( "cryfs" ) )
			devList << info;

		else
			continue;
	}

	return devList;
};

NBDeviceInfo NBDeviceManager::deviceInfoForPath( QString path ) {

	pollDevices();

	if ( isDir( path ) and not path.endsWith( "/" ) )
		path += "/";

	/* Make our path absolute */
	if ( QFileInfo( path ).isRelative() )
		path = QFileInfo( path ).absolutePath();

	if ( QFileInfo( path ).isRoot() )
		return devicesList.value( "/" );

	QString cmpPath = path;

	while ( true ) {
		/* If we have reached the root directory: This is a mount point. */
		if ( cmpPath == "/" )
			return devicesList.value( "/" );

		Q_FOREACH( NBDeviceInfo info, devicesList.values() ) {
			if ( info.mountPoint() == cmpPath )
				return info;
		}

		/* Our previously set path is not a mount point. So check if its parent is. */
		cmpPath = dirName( cmpPath );
	}

	/* Should not come here at all */
	return devicesList.value( "/" );
};

void NBDeviceManager::pollDevices() {

	/* mtab file path */
	FILE *fp = setmntent( "/etc/mtab", "r" );

	/* Entry struct */
	struct mntent *entry;

	/* Clear devices list */
	devicesList.clear();

	QStringList virtualFS;
	virtualFS << "sysfs" << "cgroup" << "cgroup2" << "proc" << "devtmpfs" << "devpts";
	virtualFS << "tmpfs" << "securityfs" << "pstore" << "autofs" << "mqueue";
	virtualFS << "debugfs" << "hugetlbfs" << "fusectl" << "fuse.gvfsd-fuse";
	virtualFS << "binfmt_misc" << "configfs" << "bpf";

	while( ( entry = getmntent( fp ) ) != NULL ) {
		/* Remove virtual mount points: dev, sys, proc etc */
		if ( virtualFS.contains( entry->mnt_type ) )
			continue;

		/* AppImage fix */
		bool yes = QString( entry->mnt_type ).contains( "AppImage" );
		yes = yes or QString( entry->mnt_type ).startsWith( "fuse." );
		yes = yes and QString( entry->mnt_dir ).startsWith( "/tmp/" );
		if ( yes )
			continue;

		/* Snap fix */
		// qDebug() << entry->mnt_fsname;
		yes = QString( entry->mnt_fsname ).contains( ".snap" );
		yes = yes and QString( entry->mnt_dir ).startsWith( "/snap/" );
		if ( yes )
			continue;

		NBDeviceInfo info( new NBDeviceInfoPrivate( entry->mnt_fsname, entry->mnt_dir, entry->mnt_type ) );
		/* Failsafe: ignore mounts with zero total size */
		if ( not info.bytesTotal() ) {
			qDebug() << info.displayName() << info.device() << info.mountPoint();
			continue;
		}

		devicesList[ entry->mnt_dir ] = info;
	};

	endmntent( fp );

	init = true;
};

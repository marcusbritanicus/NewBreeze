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

	if ( devicesList.keys().contains( path ) )
		return devicesList.value( path );

	int bestMatch = path.size();
	NBDeviceInfo bestDevice;

	Q_FOREACH( QString mt, devicesList.keys() ) {
		if ( not path.startsWith( mt ) )
			continue;

		int match = path.compare( mt );
		if ( ( match > 0 ) and ( match < bestMatch ) ) {
			bestMatch = match;
			bestDevice = devicesList.value( mt );
		}
	}

	return bestDevice;
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
	virtualFS << "binfmt_misc";

	while( ( entry = getmntent( fp ) ) != NULL ) {
		/* Remove virtual mount points: dev, sys, proc etc */
		if ( virtualFS.contains( entry->mnt_type ) )
			continue;

		NBDeviceInfo info( new NBDeviceInfoPrivate( entry->mnt_fsname, entry->mnt_dir, entry->mnt_type ) );
		devicesList[ entry->mnt_dir ] = info;
	};

	endmntent( fp );

	init = true;
};

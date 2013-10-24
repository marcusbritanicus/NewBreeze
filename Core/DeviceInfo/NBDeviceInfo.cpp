 /*
	*
	* NBDeviceInfo.cpp - DeviceInfo class for NewBreeze
	*
*/

#include <NBDeviceInfo.hpp>
#include <NBTools.hpp>
#include <sys/statvfs.h>

NBDeviceManager::NBDeviceManager() {
};

QList<NBDeviceInfo> NBDeviceManager::allDevices() {

	QList<NBDeviceInfo> devInfoList;

	foreach( QString devLine, getMounts() ) {
		NBDeviceInfo devInfo = deviceInfoForDevice( devLine.split( " " ).at( 0 ) );
		if ( devInfo.driveName() != "unknown" )
				devInfoList << devInfo;
	}

	return devInfoList;
};

NBDeviceInfo NBDeviceManager::deviceInfoForPath( QString path ) {

	NBDeviceInfoPrivate devInfoP;

	struct statvfs info;
	statvfs( path.toLocal8Bit(), &info );

	struct stat info2;
	stat( path.toLocal8Bit(), &info2 );

	QStringList vfsTypes;

	vfsTypes << "efs" << "ext" << "ext2" << "ext3" << "ext4" << "hfs" << "hfsplus" << "hpfs" << "iso9660" << "jfs";
	vfsTypes << "minix" << "msdos" << "ntfs" << "reiserfs" << "squashfs" << "smbfs" << "ubifs" << "udf" << "ufs";
	vfsTypes << "umsdos" << "usbfs" << "vfat" << "xfs" << "xiafs" << "ntfs-3g" << "fuseblk" << "fuse" << "encfs";
	vfsTypes << "fuse.encfs" << "btrfs";

	// Get the device name
	QString devById = QString( "/sys/dev/block/" ) + QString::number( major( info2.st_dev ) ) + ":" + QString::number( minor( info2.st_dev ) );

	// Get the mount point
	QRegExp mountsRx( "(\\S+) (\\S+) ([a-z0-9-.]+)" );
	QString mountsInfo = getMountsInfo( path );

	if ( mountsRx.indexIn( mountsInfo ) == -1 )
		return NBDeviceInfo();

	devInfoP.dN = mountsRx.cap( 1 );
	devInfoP.mP = mountsRx.cap( 2 );

	// If the disk is identified using uuid, get the /dev/ version
	if ( devInfoP.dN.startsWith( "/dev/disk/by-uuid" ) )
		devInfoP.dN = QFileInfo( "/dev/" + baseName( QFileInfo( devById ).symLinkTarget() ) ).symLinkTarget();

	if ( QFileInfo( devInfoP.mP ).isRoot() )
		devInfoP.dL = QString( "FileSystem" );

	else
		devInfoP.dL = getDevLabel( devInfoP.dN ).isEmpty() ? baseName( devInfoP.mP ) : getDevLabel( devInfoP.dN );

	devInfoP.fS = mountsRx.cap( 3 );
	if ( vfsTypes.contains( devInfoP.fS ) )
		devInfoP.dT = getDevType( devInfoP.dN, devInfoP.fS );

	else
		return NBDeviceInfo();

	devInfoP.fSz = ( quint64 ) ( info.f_bfree ) * info.f_frsize;
	devInfoP.aSz = ( quint64 ) ( info.f_bavail ) * info.f_frsize;
	devInfoP.uSz = ( quint64 ) ( info.f_blocks - info.f_bfree ) * info.f_frsize;
	devInfoP.dSz = ( quint64 ) ( info.f_blocks ) * info.f_frsize;

	return NBDeviceInfo( devInfoP );
};

NBDeviceInfo NBDeviceManager::deviceInfoForDevice( QString dev ) {

	NBDeviceInfoPrivate devInfoP;

	QStringList vfsTypes;

	vfsTypes << "efs" << "ext" << "ext2" << "ext3" << "ext4" << "hfs" << "hfsplus" << "hpfs" << "iso9660" << "jfs";
	vfsTypes << "minix" << "msdos" << "ntfs" << "reiserfs" << "squashfs" << "smbfs" << "ubifs" << "udf" << "ufs";
	vfsTypes << "umsdos" << "usbfs" << "vfat" << "xfs" << "xiafs" << "ntfs-3g" << "fuseblk" << "fuse" << "encfs";
	vfsTypes << "fuse.encfs" << "btrfs";

	// Get the mount point
	QRegExp mountsRx( "(\\S+) (\\S*) ([a-z0-9-.]+)" );
	QString mountsInfo = getMountsInfo( dev );

	if ( mountsRx.indexIn( mountsInfo ) == -1 )
		return NBDeviceInfo();

	devInfoP.dN = mountsRx.cap( 1 );
	devInfoP.mP = mountsRx.cap( 2 );
	devInfoP.mP.replace( "\\040", " " );

	// If the disk is identified using uuid, get the /dev/ version
	if ( devInfoP.dN.startsWith( "/dev/disk/by-uuid" ) )
		devInfoP.dN = QFileInfo( dev ).symLinkTarget();

	if ( QFileInfo( devInfoP.mP ).isRoot() )
		devInfoP.dL = QString( "FileSystem" );

	else
		devInfoP.dL = getDevLabel( devInfoP.dN ).isEmpty() ? devInfoP.mP.split( "/" ).last() : getDevLabel( devInfoP.dN );

	devInfoP.fS = mountsRx.cap( 3 );
	if ( vfsTypes.contains( devInfoP.fS ) )
		devInfoP.dT = getDevType( devInfoP.dN, devInfoP.fS );

	else
		return NBDeviceInfo();

	struct statvfs info;
	statvfs( devInfoP.mP.toLocal8Bit(), &info );

	devInfoP.fSz = ( quint64 ) ( info.f_bfree ) * info.f_frsize;
	devInfoP.aSz = ( quint64 ) ( info.f_bavail ) * info.f_frsize;
	devInfoP.uSz = ( quint64 ) ( info.f_blocks - info.f_bfree ) * info.f_frsize;
	devInfoP.dSz = ( quint64 ) ( info.f_blocks ) * info.f_frsize;

	return NBDeviceInfo( devInfoP );
};

QStringList NBDeviceManager::getMounts() {

	std::ifstream mounts;
	mounts.open( "/proc/self/mounts" );

	QStringList mountsInfo;

	while ( not mounts.eof() ) {
		char line[ 1024 ];
		mounts.getline( line, 1024 );

		mountsInfo << QString( line ).trimmed();
	}

	return mountsInfo;
}

QString NBDeviceManager::getMountsInfo( QString dev ) {

	QStringList devList = getMounts();

	// Regular device
	if ( dev.startsWith( "/dev/" ) ) {
		foreach( QString devLine, devList ) {
			// From /proc/self/mounts - Starting with @p dev
			if ( devLine.contains( dev ) )
				return devLine;

			// From /proc/self/mounts - Starting with /dev/disk/bu-uuid
			if ( QFileInfo( devLine.split( " " ).at( 0 ) ).symLinkTarget() == dev )
				return devLine;
		}
	}

	// Special device: we provide path
	else {
		QString retPt;
		foreach( QString devLine, devList ) {
			if ( not devLine.count() )
				continue;

			QString mount = devLine.split( " " ).at( 1 );
			if ( dev.contains( mount ) ) {
				if ( not retPt.contains( mount ) )
					retPt = devLine;
			}
		}

		return ( retPt.size() ? retPt : QString() );
	}

	return QString();
};

QString NBDeviceManager::getDevLabel( QString dev ) {

	QDir disks = QDir( "/dev/disk/by-label" );
	disks.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
	foreach( QString disk, disks.entryList() ) {
		QFileInfo info( disks.filePath( disk ) );
		if ( info.symLinkTarget() == dev ) {
			return disk.replace( "\\x20", " " );
		}
	}

	return QString();
};

QString NBDeviceManager::getDevType( QString dev, QString vfsType ) {

	QStringList cdTypes = QStringList() << "cdfs" << "iso9660" << "udf";
	QString devType = QString( "unknown" );

	if ( cdTypes.contains( vfsType ) )
		return "optical";

	if ( vfsType.contains( "encfs" ) )
		return "encfs";

	QDir disks = QDir( "/dev/disk/by-path" );
	disks.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
	foreach( QString disk, disks.entryList() ) {
		QFileInfo info( disks.filePath( disk ) );
		if ( info.symLinkTarget() == dev ) {
			if ( info.absoluteFilePath().contains( "usb" ) )
				return QString( "usb" );

			else
				return QString( "hdd" );
		}
	}

	if ( devType == "unknown" ) {
		/*
			*
			* Lets try /sys/block approach
			*
			* Take /dev/<dev> part of the /dev/<dev> and check if 'usb' ia part of
			* target of /sys/block/<dev>. Else check the starting of <dev> and determine the type
			*
		*/
		QString sysfsPath = QString( "/sys/block/%1" ).arg( baseName( dev ) );
		if ( readLink( sysfsPath ).contains( "usb" ) )
			return QString( "usb" );

		else {
			if ( baseName( dev ).startsWith( "sd" ) )
			// We have a generic mass storage device
				return QString( "hdd" );

			else if ( baseName( dev ).startsWith( "sr" ) )
				return QString( "optical" );

			else if ( baseName( dev ).startsWith( "se" ) or baseName( dev ).startsWith( "ses" ) )
				return QString( "enclosure" );

			else if ( baseName( dev ).startsWith( "st" ) )
				return QString( "tape" );

			else
				return devType;
		}
	}

	return devType;
};

NBDeviceInfo::NBDeviceInfo() {

	dN = QString( "unknown" );
	dL = QString( "unknown" );
	fS = QString( "unknown" );
	dT = QString( "unknown" );
	mP = QString( "unknown" );
	fSz = 0;
	aSz = 0;
	uSz = 0;
	dSz = 0;
};

NBDeviceInfo::NBDeviceInfo( const NBDeviceInfoPrivate other ) {

	dN = other.dN;
	dL = other.dL;
	fS = other.fS;
	dT = other.dT;
	mP = other.mP;
	fSz = other.fSz;
	aSz = other.aSz;
	uSz = other.uSz;
	dSz = other.dSz;
};

QString NBDeviceInfo::driveName() const {

	return dN;
};

QString NBDeviceInfo::driveLabel() const {

	return dL;
};

QString NBDeviceInfo::driveFS() {

	return fS;
};

QString NBDeviceInfo::driveType() const {

	return dT;
};

QString NBDeviceInfo::mountPoint() const {

	return mP;
};

quint64 NBDeviceInfo::freeSpace() {

	return fSz;
};

quint64 NBDeviceInfo::availSpace() {

	return aSz;
};

quint64 NBDeviceInfo::usedSpace() {

	return uSz;
};

quint64 NBDeviceInfo::driveSize() {

	return dSz;
};

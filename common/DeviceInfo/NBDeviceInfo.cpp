/*
	*
	* NBDeviceInfo.cpp - DeviceInfo class for NewBreeze
	*
*/

#include "NBDeviceInfo.hpp"

inline QString readLink( QString path ) {

	char linkTarget[ 1024 ] = { 0 };
	readlink( path.toLocal8Bit().data(), linkTarget, 1023 );

	return QString( linkTarget );
};

inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( basename( strdup( path.toLocal8Bit().data() ) ) );
};

inline QString getDevType( QString dev, QString vfsType ) {

	QStringList cdTypes = QStringList() << "cdfs" << "iso9660" << "udf";
	QString devType = QString( "unknown" );

	if ( cdTypes.contains( vfsType ) )
		return "optical";

	if ( vfsType.contains( "encfs" ) )
		return "encfs";

	if ( vfsType.contains( "archivemount" ) )
		return "archive";

	QDir disks = QDir( "/dev/disk/by-path" );
	disks.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot | QDir::System );
	foreach( QString disk, disks.entryList() ) {
		QFileInfo info( disks.filePath( disk ) );
		if ( info.symLinkTarget() == dev ) {
			if ( info.absoluteFilePath().contains( "usb" ) )
				return QString( "usb" );

			else {
				if ( vfsType.toLower().contains( "ntfs" ) )
					return QString( "hdd-win" );

				else if ( vfsType.toLower().contains( "fuseblk" ) )
					return QString( "hdd-win" );

				else if ( vfsType.toLower().contains( "ext" ) )
					return QString( "hdd-linux" );

				else if ( vfsType.toLower().contains( "jfs" ) )
					return QString( "hdd-linux" );

				else if ( vfsType.toLower().contains( "reiser" ) )
					return QString( "hdd-linux" );

				else if ( vfsType.toLower().contains( "zfs" ) )
					return QString( "hdd-linux" );

				else if ( vfsType.toLower().contains( "xfs" ) )
					return QString( "hdd-linux" );

				else if ( vfsType.toLower().contains( "btrfs" ) )
					return QString( "hdd-linux" );

				else
					return QString( "hdd" );
			}
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

inline QString getDevLabel( QString name1, QString name2 ) {

	if ( ( name1 == "/" ) or ( name2 == "/" ) )
		return "FileSystem";

	if ( not name1.isEmpty() )
		return name1.replace( "\\x20", " " );

	else if ( not name2.isEmpty() )
		return baseName( name2 ).replace( "\\x20", " " );

	else
		return "Device X";
};

NBDeviceManager::NBDeviceManager( QObject *parent ) : QObject( parent ) {

};

QList<NBDeviceInfo> NBDeviceManager::allMounts() {

	QList<NBDeviceInfo> devices;

	Q_FOREACH( QStorageInfo sInfo, QStorageInfo::mountedVolumes() ) {
		NBDeviceInfo devInfo;

		if ( sInfo.fileSystemType().contains( "gvfs" ) )
			continue;

		devInfo.dN = sInfo.device();
		devInfo.dL = getDevLabel( sInfo.name(), sInfo.displayName() );
		devInfo.fS = sInfo.fileSystemType();
		devInfo.dT = getDevType( devInfo.dN, devInfo.fS );
		devInfo.mP = sInfo.rootPath();
		devInfo.fSz = sInfo.bytesFree();
		devInfo.aSz = sInfo.bytesAvailable();
		devInfo.uSz = sInfo.bytesTotal() - sInfo.bytesFree();
		devInfo.dSz = sInfo.bytesTotal();

		devices << devInfo;
	}

	return devices;
};

QList<NBDeviceInfo> NBDeviceManager::allDrives() {

	QList<NBDeviceInfo> devices;

	Q_FOREACH( QStorageInfo sInfo, QStorageInfo::mountedVolumes() ) {
		NBDeviceInfo devInfo;

		if ( not sInfo.device().startsWith( "/dev/" ) )
			continue;

		devInfo.dN = sInfo.device();
		devInfo.dL = getDevLabel( sInfo.name(), sInfo.displayName() );
		devInfo.fS = sInfo.fileSystemType();
		devInfo.dT = getDevType( devInfo.dN, devInfo.fS );
		devInfo.mP = sInfo.rootPath();
		devInfo.fSz = sInfo.bytesFree();
		devInfo.aSz = sInfo.bytesAvailable();
		devInfo.uSz = sInfo.bytesTotal() - sInfo.bytesFree();
		devInfo.dSz = sInfo.bytesTotal();

		devices << devInfo;
	}

	return devices;
};

QList<NBDeviceInfo> NBDeviceManager::allVirtualMounts() {

	QList<NBDeviceInfo> devices;

	Q_FOREACH( QStorageInfo sInfo, QStorageInfo::mountedVolumes() ) {
		NBDeviceInfo devInfo;

		if ( sInfo.device().startsWith( "/dev" ) )
			continue;

		if ( sInfo.rootPath().startsWith( "/run" ) )
			continue;

		devInfo.dN = sInfo.device();
		devInfo.dL = getDevLabel( sInfo.name(), sInfo.displayName() );
		devInfo.fS = sInfo.fileSystemType();
		devInfo.dT = getDevType( devInfo.dN, devInfo.fS );
		devInfo.mP = sInfo.rootPath();
		devInfo.fSz = sInfo.bytesFree();
		devInfo.aSz = sInfo.bytesAvailable();
		devInfo.uSz = sInfo.bytesTotal() - sInfo.bytesFree();
		devInfo.dSz = sInfo.bytesTotal();

		devices << devInfo;
	}

	return devices;
};

NBDeviceInfo NBDeviceManager::deviceInfoForPath( QString path ) {

	NBDeviceInfo devInfo;

	QStorageInfo sInfo;
	sInfo.setPath( path );

	devInfo.dN = sInfo.device();
	devInfo.dL = getDevLabel( sInfo.name(), sInfo.displayName() );
	devInfo.fS = sInfo.fileSystemType();
	devInfo.dT = getDevType( devInfo.dN, devInfo.fS );
	devInfo.mP = sInfo.rootPath();
	devInfo.fSz = sInfo.bytesFree();
	devInfo.aSz = sInfo.bytesAvailable();
	devInfo.uSz = sInfo.bytesTotal() - sInfo.bytesFree();
	devInfo.dSz = sInfo.bytesTotal();


	if ( devInfo.mP == "/" )
		devInfo.dT = "hdd";

	return devInfo;
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

NBDeviceInfo::NBDeviceInfo( const NBDeviceInfo& other ) {

	dN = other.driveName();
	dL = other.driveLabel();
	fS = other.driveFS();
	dT = other.driveType();
	mP = other.mountPoint();
	fSz = other.freeSpace();
	aSz = other.availSpace();
	uSz = other.usedSpace();
	dSz = other.driveSize();
};

QString NBDeviceInfo::driveName() const {

	return dN;
};

QString NBDeviceInfo::driveLabel() const {

	return dL;
};

QString NBDeviceInfo::driveFS() const {

	return fS;
};

QString NBDeviceInfo::driveType() const {

	return dT;
};

QString NBDeviceInfo::mountPoint() const {

	return mP;
};

quint64 NBDeviceInfo::freeSpace() const {

	return fSz;
};

quint64 NBDeviceInfo::availSpace() const {

	return aSz;
};

quint64 NBDeviceInfo::usedSpace() const {

	return uSz;
};

quint64 NBDeviceInfo::driveSize() const {

	return dSz;
};

 /*
	*
	* NBDeviceInfo.cpp - DeviceInfo class for NewBreeze
	*
*/

#include <NBDeviceInfo.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>

inline QString readLink( QString path ) {

	char linkTarget[ 1024 ] = { 0 };
	readlink( qPrintable( path ), linkTarget, 1023 );

	return QString( linkTarget );
};

inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	return QString( basename( strdup( qPrintable( path ) ) ) );
};

inline QString getDevType( QString dev, QString vfsType ) {

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

inline QString getDevLabel( QString name1, QString name2 ) {

	if ( ( name1 == "/" ) or ( name2 == "/" ) )
		return "FileSystem";

	if ( not name1.isEmpty() )
		return name1;

	else if ( not name2.isEmpty() )
		return baseName( name2 );

	else
		return "Device X";
};

QList<NBDeviceInfo> NBDeviceManager::allDevices() {

	QList<NBDeviceInfo> devices;
	Q_FOREACH( QVolumeInfo vInfo, QVolumeInfo::volumes() )
		devices << NBDeviceManager::deviceInfoForPath( vInfo.rootPath() );

	return devices;
};

NBDeviceInfo NBDeviceManager::deviceInfoForPath( QString path ) {

	QVolumeInfo vInfo;
	vInfo.setPath( path );

	NBDeviceInfoPrivate devInfoP;

	devInfoP.dN = vInfo.device();
	devInfoP.dL = getDevLabel( vInfo.name(), vInfo.displayName() );
	devInfoP.fS = vInfo.fileSystemType();
	devInfoP.dT = getDevType( devInfoP.dN, devInfoP.fS );
	devInfoP.mP = vInfo.rootPath();
	devInfoP.fSz = vInfo.bytesFree();
	devInfoP.aSz = vInfo.bytesAvailable();
	devInfoP.uSz = vInfo.bytesTotal() - vInfo.bytesFree();
	devInfoP.dSz = vInfo.bytesTotal();

	if ( devInfoP.mP == "/" )
		devInfoP.dT = "hdd";

	return NBDeviceInfo( devInfoP );
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

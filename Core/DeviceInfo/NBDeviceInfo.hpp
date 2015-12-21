/*
	*
	* NBDeviceInfo.hpp - NBDeviceInfo.cpp class for NewBreeze
	*
*/

#pragma once
#ifndef NBDEVICEINFO_HPP
#define NBDEVICEINFO_HPP

#include <fstream>
#include <unistd.h>

#include <QtCore>

#include <Monitor.h>
#include <Partition.h>

#include <QVolumeInfo.hpp>

class NBDeviceInfoPrivate {

	public:
		QString dN;			// Drive Name ( e.g, sda1, sda2, sr0, etc )
		QString dL;			// Drive Label ( e.g, Wheezy, Users, KeyDrive, etc )
		QString fS;			// Drive FileSystem ( e.g, ext2/3/4, fat32/vfat, ntfs, etc )
		QString dT;			// Drive Type ( HDD, USB, Optical, FUSE etc )
		QString mP;			// Mount Point
		quint64 fSz;		// Free Size
		quint64 aSz;		// Available Size
		quint64 uSz;		// Used Size
		quint64 dSz;		// Drive Size
};

class NBDeviceInfo : public NBDeviceInfoPrivate {

	public:
		NBDeviceInfo();
		NBDeviceInfo( NBDeviceInfoPrivate other );

		QString driveName() const;
		QString driveLabel() const;
		QString driveFS();
		QString driveType() const;
		QString mountPoint() const;
		quint64 freeSpace();
		quint64 availSpace();
		quint64 usedSpace();
		quint64 driveSize();

	private:
		QString dN;			// Drive Name ( e.g, sda1, sda2, sr0, etc )
		QString dL;			// Drive Label ( e.g, Wheezy, Users, KeyDrive, etc )
		QString fS;			// Drive FileSystem ( e.g, ext2/3/4, fat32/vfat, ntfs, etc )
		QString dT;			// Drive Type ( HDD, USB, Optical, FUSE etc )
		QString mP;			// Mount Point
		quint64 fSz;		// Free Size
		quint64 aSz;		// Available Size
		quint64 uSz;		// Used Size
		quint64 dSz;		// Drive Size
};

Q_DECLARE_METATYPE(NBDeviceInfo);

class NBDeviceManager : public QObject {
	Q_OBJECT

	public :
		NBDeviceManager( QObject *parent = 0  );

		static QList<NBDeviceInfo> allDevices();
		static NBDeviceInfo deviceInfoForPath( QString path );

	public slots:
		void printDevInfo( const QDBusObjectPath & );

	Q_SIGNALS :
		void updateDevices();
};

#endif

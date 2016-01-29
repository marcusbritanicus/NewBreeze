/*
	*
	* NBDeviceManager2.cpp - DeviceManager for NewBreeze
	*
*/

#pragma once

#include <QtCore>

#include <Monitor.h>
#include <Partition.h>

class NBDeviceInfo2 {

	public:
		NBDeviceInfo2();

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

Q_DECLARE_METATYPE(NBDeviceInfo2);

class NBDeviceManager2 : public QObject {
	Q_OBJECT

	friend NBDeviceInfo2;

	public :
		NBDeviceManager2( QObject *parent = 0  );

		static QList<NBDeviceInfo2> allDevices();
		static NBDeviceInfo2 deviceInfoForPath( QString path );

	// public slots:
		// void printDevInfo( const QDBusObjectPath & );

	Q_SIGNALS :
		void updateDevices();
};

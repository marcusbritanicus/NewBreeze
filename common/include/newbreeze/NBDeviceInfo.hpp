/*
	*
	* NBDeviceInfo.hpp - NBDeviceInfo Class Header
	*
*/

#pragma once

#include <newbreeze/common.hpp>

class NBDeviceInfoPrivate;

class NBDeviceInfo {

	public:
		NBDeviceInfo();
		explicit NBDeviceInfo(const QString path);
		NBDeviceInfo(const NBDeviceInfo &other);
		~NBDeviceInfo();

		NBDeviceInfo &operator=(const NBDeviceInfo &other);

		QString mountPoint() const;
		QString device() const;
		QString fileSystemType() const;
		QString deviceType() const;
		QString displayName() const;

		qint64 bytesTotal() const;
		qint64 bytesUsed() const;
		qint64 bytesAvailable() const;
		int blockSize() const;

		bool isReadOnly() const;
		bool isValid() const;

	private:
		// friend class NBDeviceInfoPrivate;
		friend class NBDeviceManager;
		friend bool operator==(const NBDeviceInfo &first, const NBDeviceInfo &second);
		QExplicitlySharedDataPointer<NBDeviceInfoPrivate> d;

		NBDeviceInfo( NBDeviceInfoPrivate *infoPriv );
};

inline bool operator==( const NBDeviceInfo &first, const NBDeviceInfo &second ) {
    if ( first.d == second.d )
        return true;

    return first.device() == second.device();
};

inline bool operator!=( const NBDeviceInfo &first, const NBDeviceInfo &second ) {

    return !( first == second );
}

Q_DECLARE_METATYPE(NBDeviceInfo)

class NBDeviceManager : public QObject {
	Q_OBJECT

	public :
		/* List of all mounted volumes: drives + vfs */
		static QList<NBDeviceInfo> allMounts();

		/* List of for all mounted drives */
		static QList<NBDeviceInfo> allDrives();

		/* List of all mounted vfs */
		static QList<NBDeviceInfo> allVirtualMounts();

		/* List of all mounted vfs */
		static QList<NBDeviceInfo> allEncryptedMounts();

		/* Volume info for path */
		static NBDeviceInfo deviceInfoForPath( QString path );

	private:
		/* Reload all the devices */
		static void pollDevices();

		static QHash<QString, NBDeviceInfo> devicesList;
		static bool init;
};

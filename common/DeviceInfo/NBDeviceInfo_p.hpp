/*
	*
	* NBDeviceInfo_p.hpp - NBDeviceInfo	 Private Class Header
	*
*/

#pragma once

#include <newbreeze/common.hpp>
#include <newbreeze/NBDeviceInfo.hpp>
#include <newbreeze/NBTools.hpp>

class NBDeviceInfoPrivate : public QSharedData {
	public:
		NBDeviceInfoPrivate();
		NBDeviceInfoPrivate( QString dev, QString mountPt, QString fs );
		bool isValid();

	private:
		void retrieveVolumeInfo();
		void getDeviceLabel();
		void getDeviceType( QString, QString );

	public:
		/* Device block path */
		QString device;

		/* File System Type */
		QString fileSystemType;

		/* Device Type */
		QString deviceType;

		/* Display Label */
		QString label;

		/* Mounted path */
		QString mountPoint;

		/* Device size, used size and available size */
		qint64 bytesTotal;
		qint64 bytesUsed;
		qint64 bytesAvailable;

		/* FS block size */
		int blockSize;

		/* Is the FS mounted readonly */
		bool readOnly;

		/* Invalid flag */
		bool mIsValid;
};

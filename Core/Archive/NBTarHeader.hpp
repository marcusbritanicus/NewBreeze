/*
	*
	* NBTarHeader.hpp - Header implementation for TAR
	*
*/

#ifndef NBTarHEADER_HPP
#define NBTarHEADER_HPP

// SystemWide Headers
#include <QtCore>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

class TarHeader {

	public:
		enum HeaderField {
			Name = 0x46c892,				// Name of the inode
			Mode,							// Mode of the inode
			Uid,							// User id of the inode
			Gid,							// Group id of the inode
			Size,							// Size of the inode
			Time,							// Modification time of the inode
			CheckSum,						// Header checksum
			Type,							// Type of the inode
			Target,							// If inode is a link, link target
			Magic,							// Header Magic: Always ustar\x00
			Version,						// Header Version: Always 00
			User,							// User name of the inode
			Group,							// Group name of the inode
			Major,							// Major Number of the original device
			Minor,							// Minor Number of the original device
			Prefix,							// If the name is too long, store the first part here
			Padding,						// 12 \x00 bytes
		};

		/* Create a tar header for a file */
		static TarHeader* createHeader( QString );

		/* Parse the given byte array to get the tar header */
		static TarHeader* parseHeader( QByteArray );

		/* Process and return the header as a bytearray for writing */
		QByteArray headerString();

		/* Value of the given field */
		QVariant value( TarHeader::HeaderField );

		/* Is this a valid header */
		bool isValid();

	private:
		TarHeader() {
			mIsValid = false;
		}

		void computeChecksum();
		void validateHeader();

		QByteArray header;
		QByteArray name, mode, uid, gid, size, mtime, chksum, type, target, magic, version, uname, gname, major, minor, prefix, padding;

		bool mIsValid;

		static TarHeader *mHeader;
};

#endif

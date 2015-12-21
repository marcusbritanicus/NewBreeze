/*
	*
	* NBTar.hpp - NBTar Header
	*
*/

#ifndef NBTarUTILS_HPP
#define NBTarUTILS_HPP

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
#include <libgen.h>

/* Check if the given path is a directory */
bool isDir( QString );

/* Qt4 Equivalent of linux dirname and basename */
QString dirName( QString path );
QString baseName( QString path );

/* Check if a file exists */
bool exists( QString );

/* Check if the file is readable */
bool isReadable( QString );

#endif

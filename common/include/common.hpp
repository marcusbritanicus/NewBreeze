/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

// STL
#include <iostream>
#include <fstream>

// LibArchive
#include <archive.h>
#include <archive_entry.h>

// Qt Headers
#include <QtCore>
// C++ Standard Library
#include <fstream>

// C Standard Library
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

// SystemWide Headers
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <utime.h>
#include <errno.h>
#include <sys/vfs.h>
#include <libgen.h>
#include <magic.h>
#include <sys/inotify.h>
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#if QT_VERSION >= 0x050000
	#include <QMimeDatabase>
	#include <QMimeType>
	#include <QStandardPaths>
#else
	#include "QMimeDatabase.hpp"
	#include "QMimeType.hpp"
	#include "QStandardPaths.hpp"
#endif

#if QT_VERSION >= 0x050100
	#include <QCryptographicHash>
	#define QCryptographicHash5 QCryptographicHash
#else
	#include "QCryptographicHash5.hpp"
#endif

#if QT_VERSION < 0x050400
	#include "QStorageInfo.hpp"
#else
	#include <QStorageInfo>
#endif

#if defined COMMON
	#define NBCOMMON_DLLSPEC Q_DECL_EXPORT
#else
	#define NBCOMMON_DLLSPEC Q_DECL_IMPORT
#endif

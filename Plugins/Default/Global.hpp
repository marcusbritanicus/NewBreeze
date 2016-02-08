/*
    *
    * Global.hpp - Globally used header
    *
*/

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <iostream>
#include <fstream>

// Qt Headers
#include <QtGui>
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
	#include <QtWidgets>
	#include <QMimeDatabase>
	#include <QMimeType>
	#include <QStandardPaths>
#else
	#include <NBMimeDatabase.hpp>
	#include <NBMimeType.hpp>
	#include <NBStandardPaths.hpp>
#endif

static QMimeDatabase mimeDb;

#endif

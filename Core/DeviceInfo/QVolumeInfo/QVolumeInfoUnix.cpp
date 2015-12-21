/****************************************************************************
**
** Copyright (C) 2014 Ivan Komissarov
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QVolumeInfoP.hpp>

#include <QtCore>
#include <QtCore/qfileinfo.h>
#include <QtCore/qtextstream.h>

#define EINTR_LOOP(var, cmd) do { var = cmd; } while (var == -1 && errno == EINTR)

#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#if defined(Q_OS_BSD4)
#  include <sys/mount.h>
#elif defined(Q_OS_LINUX)
#  include <mntent.h>
#  include <sys/statvfs.h>
#elif defined(Q_OS_ANDROID)
#  include <sys/mount.h>
#  include <mntent.h>
#elif defined(Q_OS_SOLARIS)
#  include <sys/mnttab.h>
#endif

#if defined(Q_OS_BSD4)
#  define QT_STATFSBUF struct statvfs
#  define QT_STATFS    ::statvfs
#elif defined(Q_OS_ANDROID)
#  define QT_STATFS    ::statfs
#  define QT_STATFSBUF struct statfs
#else
#  if defined(QT_LARGEFILE_SUPPORT)
#    define QT_STATFSBUF struct statvfs64
#    define QT_STATFS    ::statvfs64
#  else
#    define QT_STATFSBUF struct statvfs
#    define QT_STATFS    ::statvfs
#  endif // QT_LARGEFILE_SUPPORT
#endif // Q_OS_BSD4

#if QT_VERSION <= 0x050000
	# define Q_NULLPTR         NULL
#endif

QT_BEGIN_NAMESPACE

static bool isPseudoFs(const QString &mountDir, const QByteArray &type)
{
    if (type.startsWith('/'))
        return false;

	#if QT_VERSION >= 0x050000
		if (mountDir.startsWith(QStringLiteral("/dev"))
			|| mountDir.startsWith(QStringLiteral("/proc"))
			|| mountDir.startsWith(QStringLiteral("/run"))
			|| mountDir.startsWith(QStringLiteral("/sys"))
			|| mountDir.startsWith(QStringLiteral("/var/run"))
			|| mountDir.startsWith(QStringLiteral("/var/lock"))) {
			return true;
	#else
		if (mountDir.startsWith(QString("/dev"))
			|| mountDir.startsWith(QString("/proc"))
			|| mountDir.startsWith(QString("/run"))
			|| mountDir.startsWith(QString("/sys"))
			|| mountDir.startsWith(QString("/var/run"))
			|| mountDir.startsWith(QString("/var/lock"))) {
			return true;
	#endif
    }

#if defined(Q_OS_LINUX)
    if (type == "rootfs")
        return true;
#endif

    return false;
}

class QVolumeIterator
{
public:
    QVolumeIterator();
    ~QVolumeIterator();

    inline bool isValid() const;
    inline bool next();
    inline QString rootPath() const;
    inline QByteArray fileSystemType() const;
    inline QByteArray device() const;
private:
#if defined(Q_OS_BSD4)
    statfs *stat_buf;
    int entryCount;
    int currentIndex;
#elif defined(Q_OS_LINUX)
    FILE *fp;
    mntent mnt;
    QByteArray buffer;
#elif defined(Q_OS_SOLARIS)
    FILE *fp;
    mnttab mnt;
#endif
};

#if defined(Q_OS_BSD4)

inline QVolumeIterator::QVolumeIterator()
    : entryCount(::getmntinfo(&stat_buf, 0)),
      currentIndex(-1)
{
}

inline QVolumeIterator::~QVolumeIterator()
{
}

inline bool QVolumeIterator::isValid() const
{
    return entryCount != -1;
}

inline bool QVolumeIterator::next()
{
    return ++currentIndex < entryCount;
}

inline QString QVolumeIterator::rootPath() const
{
    return QFile::decodeName(stat_buf[currentIndex].f_mntonname);
}

inline QByteArray QVolumeIterator::fileSystemType() const
{
    return QByteArray(stat_buf[currentIndex].f_fstypename);
}

inline QByteArray QVolumeIterator::device() const
{
    return QByteArray(stat_buf[currentIndex].f_mntfromname);
}

#elif defined(Q_OS_SOLARIS)

static const char pathMounted[] = "/etc/mtab";

inline QVolumeIterator::QVolumeIterator()
{
    const int fd = qt_safe_open(pathMounted, O_RDONLY);
    fp = ::fdopen(fd, "r");
}

inline QVolumeIterator::~QVolumeIterator()
{
    if (fp)
        ::fclose(fp);
}

inline bool QVolumeIterator::isValid() const
{
    return fp != Q_NULLPTR;
}

inline bool QVolumeIterator::next()
{
    return ::getmntent(fp, &mnt) == Q_NULLPTR;
}

inline QString QVolumeIterator::rootPath() const
{
    return QFile::decodeName(mnt->mnt_mountp);
}

inline QByteArray QVolumeIterator::fileSystemType() const
{
    return QByteArray(mnt->mnt_fstype);
}

inline QByteArray QVolumeIterator::device() const
{
    return QByteArray(mnt->mnt_mntopts);
}

#else

static const char pathMounted[] = "/etc/mtab";
static const int bufferSize = 3*PATH_MAX; // 2 paths (mount point+device) and metainfo

inline QVolumeIterator::QVolumeIterator() :
    buffer(bufferSize, 0)
{
#if defined(Q_OS_ANDROID)
    const int fd = qt_safe_open(pathMounted, O_RDONLY);
    fp = ::fdopen(fd, "r");
#else
    fp = ::setmntent(pathMounted, "r");
#endif
}

inline QVolumeIterator::~QVolumeIterator()
{
#if defined(Q_OS_ANDROID)
    if (fp)
        ::fclose(fp);
#else
    if (fp)
        ::endmntent(fp);
#endif
}

inline bool QVolumeIterator::isValid() const
{
    return fp != Q_NULLPTR;
}

inline bool QVolumeIterator::next()
{
    return ::getmntent_r(fp, &mnt, buffer.data(), buffer.size()) != Q_NULLPTR;
}

inline QString QVolumeIterator::rootPath() const
{
    return QFile::decodeName(mnt.mnt_dir);
}

inline QByteArray QVolumeIterator::fileSystemType() const
{
    return QByteArray(mnt.mnt_type);
}

inline QByteArray QVolumeIterator::device() const
{
    return QByteArray(mnt.mnt_fsname);
}

#endif

void QVolumeInfoPrivate::initRootPath()
{
    rootPath = QFileInfo(rootPath).canonicalFilePath();

    if (rootPath.isEmpty())
        return;

    QVolumeIterator it;
    if (!it.isValid()) {
		#if QT_VERSION >= 0x050000
			rootPath = QStringLiteral("/");
		#else
			rootPath = QString( "/" );
		#endif
        return;
    }

    int maxLength = 0;
    const QString oldRootPath = rootPath;
    rootPath.clear();

    while (it.next()) {
        const QString mountDir = it.rootPath();
        const QByteArray fsName = it.fileSystemType();
        if (isPseudoFs(mountDir, fsName))
            continue;
        // we try to find most suitable entry
        if (oldRootPath.startsWith(mountDir) && maxLength < mountDir.length()) {
            maxLength = mountDir.length();
            rootPath = mountDir;
            device = it.device();
            fileSystemType = fsName;
        }
    }
}

static inline QString retrieveLabel(const QByteArray &device)
{
#ifdef Q_OS_LINUX
    static const char pathDiskByLabel[] = "/dev/disk/by-label";

    QDirIterator it(QLatin1String(pathDiskByLabel), QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.fileInfo());
        if (fileInfo.isSymLink() && fileInfo.symLinkTarget().toLocal8Bit() == device)
            return fileInfo.fileName();
    }
#else
    Q_UNUSED(device);
#endif

    return QString();
}

void QVolumeInfoPrivate::doStat(uint requiredFlags)
{
    if (getCachedFlag(requiredFlags))
        return;

    if (!getCachedFlag(CachedRootPathFlag | CachedDeviceFlag | CachedFileSystemTypeFlag)) {
        initRootPath();
        setCachedFlag(CachedRootPathFlag | CachedDeviceFlag | CachedFileSystemTypeFlag);
    }

    if (rootPath.isEmpty() || (getCachedFlag(CachedValidFlag) && !valid))
        return;

    if (!getCachedFlag(CachedValidFlag))
        requiredFlags |= CachedValidFlag; // force volume validation

    uint bitmask = CachedBytesTotalFlag | CachedBytesFreeFlag | CachedBytesAvailableFlag
            | CachedReadOnlyFlag | CachedReadyFlag | CachedValidFlag;
    if (requiredFlags & bitmask) {
        retreiveVolumeInfo();
        setCachedFlag(bitmask);

        if (!valid)
            return;
    }

    bitmask = CachedLabelFlag;
    if (requiredFlags & bitmask) {
        name = retrieveLabel(device);
        setCachedFlag(bitmask);
    }
}

void QVolumeInfoPrivate::retreiveVolumeInfo()
{
    QT_STATFSBUF statfs_buf;
    int result;
    EINTR_LOOP(result, QT_STATFS(QFile::encodeName(rootPath).constData(), &statfs_buf));
    if (result == 0) {
        valid = true;
        ready = true;

        bytesTotal = statfs_buf.f_blocks * statfs_buf.f_bsize;
        bytesFree = statfs_buf.f_bfree * statfs_buf.f_bsize;
        bytesAvailable = statfs_buf.f_bavail * statfs_buf.f_bsize;

#if defined(Q_OS_ANDROID)
        readOnly = (statfs_buf.f_flags & 1 /* MS_RDONLY */) != 0;
#else
        readOnly = (statfs_buf.f_flag & ST_RDONLY) != 0;
#endif
    }
}

QList<QVolumeInfo> QVolumeInfoPrivate::volumes()
{
    QVolumeIterator it;
    if (!it.isValid())
        return QList<QVolumeInfo>() << rootVolume();

    QList<QVolumeInfo> volumes;

    while (it.next()) {
        const QString mountDir = it.rootPath();
        const QByteArray fsName = it.fileSystemType();
        if (isPseudoFs(mountDir, fsName))
            continue;

        QVolumeInfoPrivate *data = new QVolumeInfoPrivate;
        data->rootPath = mountDir;
        data->device = QByteArray(it.device());
        data->fileSystemType = fsName;
        data->setCachedFlag(CachedRootPathFlag |
                            CachedFileSystemTypeFlag |
                            CachedDeviceFlag);
        volumes.append(QVolumeInfo(*data));
    }

    return volumes;
}

QVolumeInfo QVolumeInfoPrivate::rootVolume()
{
	#if QT_VERSION >= 0x050000
		return QVolumeInfo(QStringLiteral("/"));
	#else
		return QVolumeInfo(QString("/"));
	#endif
}

QT_END_NAMESPACE

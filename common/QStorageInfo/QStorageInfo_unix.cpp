/****************************************************************************
**
** Copyright (C) 2014 Ivan Komissarov <ABBAPOH@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "QStorageInfo_p.hpp"

#include <QtCore/qdiriterator.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qtextstream.h>

#include <errno.h>
#include <sys/stat.h>

#include <mntent.h>
#include <sys/statvfs.h>

#if defined(QT_LARGEFILE_SUPPORT)
#  define QT_STATFSBUF struct statvfs64
#  define QT_STATFS    ::statvfs64
#else
#  define QT_STATFSBUF struct statvfs
#  define QT_STATFS    ::statvfs
#endif // QT_LARGEFILE_SUPPORT

QT_BEGIN_NAMESPACE

class QStorageIterator
{
public:
    QStorageIterator();
    ~QStorageIterator();

    inline bool isValid() const;
    inline bool next();
    inline QString rootPath() const;
    inline QByteArray fileSystemType() const;
    inline QByteArray device() const;
private:
    FILE *fp;
    mntent mnt;
    QByteArray buffer;
};

template <typename String>
static bool isParentOf(const String &parent, const QString &dirName)
{
    return dirName.startsWith(parent) &&
            (dirName.size() == parent.size() || dirName.at(parent.size()) == QLatin1Char('/') ||
             parent.size() == 1);
}

static bool isPseudoFs(const QStorageIterator &it)
{
    QString mountDir = it.rootPath();
    if (isParentOf(QString("/dev"), mountDir)
        || isParentOf(QString("/proc"), mountDir)
        || isParentOf(QString("/sys"), mountDir)
        || isParentOf(QString("/var/run"), mountDir)
        || isParentOf(QString("/var/lock"), mountDir)) {
        return true;
    }

    QByteArray type = it.fileSystemType();
    if (type == "tmpfs")
        return false;
    if (type == "rootfs" || type == "rpc_pipefs")
        return true;

    if (!it.device().startsWith('/'))
        return true;

    return false;
}

static const char pathMounted[] = "/etc/mtab";
static const int bufferSize = 1024; // 2 paths (mount point+device) and metainfo;
                                    // should be enough

inline QStorageIterator::QStorageIterator() :
    buffer(QByteArray(bufferSize, 0))
{
    fp = ::setmntent(pathMounted, "r");
}

inline QStorageIterator::~QStorageIterator()
{
    if (fp)
        ::endmntent(fp);
}

inline bool QStorageIterator::isValid() const
{
    return fp != 0;
}

inline bool QStorageIterator::next()
{
    return ::getmntent_r(fp, &mnt, buffer.data(), buffer.size()) != 0;
}

inline QString QStorageIterator::rootPath() const
{
    return QFile::decodeName(mnt.mnt_dir);
}

inline QByteArray QStorageIterator::fileSystemType() const
{
    return QByteArray(mnt.mnt_type);
}

inline QByteArray QStorageIterator::device() const
{
    return QByteArray(mnt.mnt_fsname);
}


void QStorageInfoPrivate::initRootPath()
{
    rootPath = QFileInfo(rootPath).canonicalFilePath();

    if (rootPath.isEmpty())
        return;

    QStorageIterator it;
    if (!it.isValid()) {
        rootPath = QString("/");
        return;
    }

    int maxLength = 0;
    const QString oldRootPath = rootPath;
    rootPath.clear();

    while (it.next()) {
        const QString mountDir = it.rootPath();
        const QByteArray fsName = it.fileSystemType();
        // we try to find most suitable entry
        if (isParentOf(mountDir, oldRootPath) && maxLength < mountDir.length()) {
            maxLength = mountDir.length();
            rootPath = mountDir;
            device = it.device();
            fileSystemType = fsName;
        }
    }
}

static inline QString retrieveLabel(const QByteArray &device)
{
    static const char pathDiskByLabel[] = "/dev/disk/by-label";

    QFileInfo devinfo(QFile::decodeName(device));
    QString devicePath = devinfo.canonicalFilePath();

    QDirIterator it(QLatin1String(pathDiskByLabel), QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.fileInfo());
        if (fileInfo.isSymLink() && fileInfo.symLinkTarget() == devicePath)
            return fileInfo.fileName();
    }

    return QString();
}

void QStorageInfoPrivate::doStat()
{
    initRootPath();
    if (rootPath.isEmpty())
        return;

    retrieveVolumeInfo();
    name = retrieveLabel(device);
}

void QStorageInfoPrivate::retrieveVolumeInfo()
{
    QT_STATFSBUF statfs_buf;
    int result;
    EINTR_LOOP(result, QT_STATFS(QFile::encodeName(rootPath).constData(), &statfs_buf));
    if (result == 0) {
        valid = true;
        ready = true;

#if defined(Q_OS_INTEGRITY) || (defined(Q_OS_BSD4) && !defined(Q_OS_NETBSD))
        bytesTotal = statfs_buf.f_blocks * statfs_buf.f_bsize;
        bytesFree = statfs_buf.f_bfree * statfs_buf.f_bsize;
        bytesAvailable = statfs_buf.f_bavail * statfs_buf.f_bsize;
#else
        bytesTotal = statfs_buf.f_blocks * statfs_buf.f_frsize;
        bytesFree = statfs_buf.f_bfree * statfs_buf.f_frsize;
        bytesAvailable = statfs_buf.f_bavail * statfs_buf.f_frsize;
#endif
        blockSize = statfs_buf.f_bsize;
#if defined(Q_OS_ANDROID) || defined(Q_OS_BSD4) || defined(Q_OS_INTEGRITY)
    #if defined(_STATFS_F_FLAGS)
        readOnly = (statfs_buf.f_flags & ST_RDONLY) != 0;
    #endif
#else
        readOnly = (statfs_buf.f_flag & ST_RDONLY) != 0;
#endif
    }
}

QList<QStorageInfo> QStorageInfoPrivate::mountedVolumes()
{
    QStorageIterator it;
    if (!it.isValid())
        return QList<QStorageInfo>() << root();

    QList<QStorageInfo> volumes;

    while (it.next()) {
        if (isPseudoFs(it))
            continue;

        const QString mountDir = it.rootPath();
        volumes.append(QStorageInfo(mountDir));
    }

    return volumes;
}

QStorageInfo QStorageInfoPrivate::root()
{
    return QStorageInfo(QString("/"));
}

QT_END_NAMESPACE

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

#ifndef QVOLUMEINFO_P_H
#define QVOLUMEINFO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QVolumeInfo.hpp>

QT_BEGIN_NAMESPACE

class QVolumeInfoPrivate : public QSharedData
{
public:
    inline QVolumeInfoPrivate() : QSharedData(),
        bytesTotal(0), bytesFree(0), bytesAvailable(0),
        readOnly(false), ready(false), valid(false),
        cachedFlags(0)
    {}

    enum CachedFlags {
        CachedRootPathFlag = 0x001,
        CachedDeviceFlag = 0x002,
        CachedFileSystemTypeFlag = 0x004,
        CachedLabelFlag = 0x008,
        CachedBytesTotalFlag = 0x010,
        CachedBytesFreeFlag = 0x020,
        CachedBytesAvailableFlag = 0x040,
        CachedReadOnlyFlag = 0x100,
        CachedReadyFlag = 0x200,
        CachedValidFlag = 0x400
    };

    inline void clear()
    {
        device.clear();
        fileSystemType.clear();
        name.clear();

        bytesTotal = 0;
        bytesFree = 0;
        bytesAvailable = 0;

        readOnly = false;
        ready = false;
        valid = false;

        cachedFlags = 0;
    }

    inline bool getCachedFlag(uint c) const
    { return !!(cachedFlags & c); }
    inline void setCachedFlag(uint c)
    { cachedFlags |= c; }

    void initRootPath();
    void doStat(uint requiredFlags);

    static QList<QVolumeInfo> volumes();
    static QVolumeInfo rootVolume();

    static void ensureCached(const QVolumeInfo *q, uint flags);

protected:
#if defined(Q_OS_WIN)
    void retreiveVolumeInfo();
    void retreiveDiskFreeSpace();
#elif defined(Q_OS_MAC)
    void retrievePosixInfo();
    void retrieveUrlProperties(bool initRootPath = false);
    void retrieveLabel();
#elif defined(Q_OS_UNIX)
    void retreiveVolumeInfo();
#endif

public:
    QString rootPath;
    QByteArray device;
    QByteArray fileSystemType;
    QString name;

    qint64 bytesTotal;
    qint64 bytesFree;
    qint64 bytesAvailable;

    ushort readOnly : 1;
    ushort ready : 1;
    ushort valid : 1;
    ushort reserved : 5;

    uint cachedFlags;
};

QT_END_NAMESPACE

#endif // QVOLUMEINFO_P_H

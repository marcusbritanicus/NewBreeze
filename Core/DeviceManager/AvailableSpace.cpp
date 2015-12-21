/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#include "AvailableSpace.h"
#include <sys/statvfs.h>

quint64 QDrive::availableSpace(const QString &fileSystemPath)
{
	QByteArray rawPath = fileSystemPath.toUtf8();
	struct statvfs stat;
	if (statvfs(rawPath.constData(), &stat))
		return 0;

	return stat.f_bavail * stat.f_bsize;
}

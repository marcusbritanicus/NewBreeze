/**
 * \author Andreas Misje
 * \date 2014
 * \copyright MIT
 */

#ifndef AVAILABLESPACE_H
#define AVAILABLESPACE_H

#include <QtCore>
#include <QString>

namespace QDrive
{
	/** \brief Return the number of available bytes on the mounted filesystem
	 *
	 * \param fileSystemPath path to any file or directory in the filesystem in
	 * question
	 */
	quint64 availableSpace(const QString &fileSystemPath);
};

#endif // AVAILABLESPACE_H

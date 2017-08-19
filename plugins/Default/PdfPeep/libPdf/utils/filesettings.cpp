/***************************************************************************
 *   Copyright (C) 2012 by Glad Deschrijver                                *
 *     <glad.deschrijver@gmail.com>                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "common.hpp"
#include "filesettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QDesktopServices>

FileSettings::FileSettings(const QString &fileName, QSettings::Format format, QObject *parent)
	: QSettings(fileSettingsPath(fileName), format, parent)
{
	Q_ASSERT_X(!QCoreApplication::organizationName().isEmpty() && !QCoreApplication::applicationName().isEmpty(), "FileSettings", "in order to not dump the file specific settings in files labeled \"Unknown Organization\", you should set the application name and organization name with QCoreApplication::setApplicationName() and QCoreApplication::setOrganizationName()");

	// TODO make sure that the file settings cache on disk doesn't become too large
}

FileSettings::~FileSettings()
{
}

void FileSettings::constrainCacheToMaxSize(int size)
{
	const QString dataPath = dataFilePath();
	QDir dataDir(dataPath);

	// get size of cache
	QFileInfoList dataFileInfoList = dataDir.entryInfoList(QDir::Files | QDir::NoDotDot, QDir::Time);
	int cacheSize = QFileInfo(dataPath).size();
	for (int i = 0; i < dataFileInfoList.size(); ++i)
		cacheSize += dataFileInfoList.at(i).size();

	while (cacheSize > size) // remove oldest file settings from cache
	{
		if (dataFileInfoList.isEmpty())
			break;
		const QString fileName = dataFileInfoList.last().fileName();
		int fileSize = dataFileInfoList.takeLast().size();
		if (dataDir.remove(fileName))
			cacheSize -= fileSize;
	}
}

/***************************************************************************/
// Data storage location

QString FileSettings::dataFilePath()
{
	QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	if (directory.isEmpty())
		directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
	directory += QLatin1String("/filesettings");
	if (!QFile::exists(directory))
	{
		QDir dir;
		dir.mkpath(directory);
	}
	return directory;
}

QString FileSettings::fileSettingsPath(const QString &fileName)
{
	QString filePath = fileName;
	return dataFilePath() + QLatin1Char('/') + (filePath.replace(QLatin1String("/"), QLatin1String("_")) + QLatin1String(".ini"));
}

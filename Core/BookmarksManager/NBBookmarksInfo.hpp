/*
	*
	* NBBookmarksInfo.hpp - NBDeviceInfo.cpp class for NewBreeze
	*
*/

#pragma once

#include <fstream>
#include <unistd.h>

#include <Global.hpp>
#include <NBTools.hpp>

class NBBookmarkInfo {

	public:
		QString displayLabel;
		QString displayIcon;
		QString mountPoint;
};

Q_DECLARE_METATYPE(NBBookmarkInfo);

class NBBookmarksInfo {

	public:
		static QList<NBBookmarkInfo> allBookmarks();
};

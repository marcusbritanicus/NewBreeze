/*
	*
	* NBBookmarkInfo.hpp - NBBookmarkInfo.cpp header file
	*
*/

#pragma once

#include "Global.hpp"

class NBBookmarkInfo {

	public:
		QString displayLabel;
		QString displayIcon;
		QString mountPoint;

		static QList<NBBookmarkInfo> allBookmarks();
};

Q_DECLARE_METATYPE(NBBookmarkInfo);

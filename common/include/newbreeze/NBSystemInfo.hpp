/*
	*
	* NBSystemInfo.hpp - NBSystemInfo.cpp header
	*
*/

#pragma once

#include <QtCore>

class NBDriveInfo;

namespace DesktopSession {
	enum Session {
		KDE,
		KDE4 = KDE,
		KDE5,
		GNOME,
		XFCE,
		RAZOR,
		LXDE,
		LXQT,
		CINNAMON,
		MATE,
		ENLIGHTENMENT,
		UNKNOWN
	};

	Session activeSession();
};

QString NBSystemIconTheme();

namespace XDG {
	enum Location {
		Desktop,
		Documents,
		Music,
		Videos,
		Movies,
		Pictures,
		Temp,
		Home,
		Config,
		Cache
	};

	QString getLocation( Location );
};

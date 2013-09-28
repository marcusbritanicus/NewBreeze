/*
	*
	* NBSystemInfo.hpp - NBSystemInfo.cpp header
	*
*/

#ifndef NBSYSTEMINFO_HPP
#define NBSYSTEMINFO_HPP

#include <QtCore>

class NBDriveInfo;

namespace DesktopSession {
	enum Session {
		KDE,
		GNOME,
		XFCE,
		RAZOR,
		LXDE,
		OBOX,
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

#endif

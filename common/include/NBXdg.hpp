/*
	*
	* NBXdg.hpp - NBXdg class header
	*
*/

#pragma once

#include "common.hpp"
#include "NBDeviceInfo.hpp"

class NBCOMMON_DLLSPEC NBXdg {
	public:
		enum XdgUserDir {
			XDG_DATA_HOME    = 0x02544878,
			XDG_CONFIG_HOME,
			XDG_CACHE_HOME
		};

		enum XdgSystemDirs {
			XDG_DATA_DIRS    = 0x196BB115,
			XDG_CONFIG_DIRS
		};

		static QString home();
		static QString xdgDefaultApp( QString );
		static QString userDir( NBXdg::XdgUserDir );
		static QStringList systemDirs( NBXdg::XdgSystemDirs );

		static QString trashLocation( QString path );
		static QString homeTrashLocation();
};

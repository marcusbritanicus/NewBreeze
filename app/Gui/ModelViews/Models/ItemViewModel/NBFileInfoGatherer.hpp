/*
	*
	* NBFileInfoGatherer.hpp - NewBreeze FileInfoGatherer Class Header
	*
*/

#pragma once

#include "Global.hpp"

class NBQuickFileInfoGatherer {

	public:
		NBQuickFileInfoGatherer();
		QVariantList getQuickFileInfo( QString path );

	private:
		QString permStr( mode_t fileMode );
		QString formatSize( qint64 num );

		static QSettings *fileTypes;
};

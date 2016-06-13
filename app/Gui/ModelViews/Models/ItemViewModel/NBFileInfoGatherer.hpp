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

		QSettings *fileTypes;
};

class NBFileInfoGatherer : public QThread {
	Q_OBJECT

	public:
		NBFileInfoGatherer( bool *term );
		~NBFileInfoGatherer();

		void gatherInfo( QStringList, QString );

	private:
		void run();

		QStringList entryList;
		QString rootPath;

		bool *__terminate;

	signals:
		void done( QString, QString, QStringList );
};

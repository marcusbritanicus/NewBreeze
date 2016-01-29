/*
	*
	* NBArchive.hpp - NBArchive.cpp header
	*
*/

#pragma once

#include <Global.hpp>

#include <NBTools.hpp>
#include <NBLibBZip2.hpp>
#include <NBLibGZip.hpp>
#include <NBLibLzma.hpp>

class NBArchive {

	public:
		NBArchive( QString );

		// Convinience Functions
		void updateInputFiles( QStringList );
		void setWorkingDir( QString );
		void setDestination( QString );

		// Workers
		void create();
		int extract();

	private:
		int copyData( struct archive *ar, struct archive *aw );
		int setFilterFormat( struct archive *ar, QMimeType mType );

		QString archiveName;

		QStringList inputList;
		QString dest;
		QString src;
};

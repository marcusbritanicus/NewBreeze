/*
	*
	* LibGZip.hpp - LibGZip.cpp header
	*
*/

#pragma once

#include <QtCore>
#include <fstream>
#include <zlib.h>

class NBGZip {

	public:
		NBGZip( QString, QString file = QString() );
		bool extract();

		static QString fileName;
		static QString gzFileName;

	private:
		gzFile gzip;
		int NBGZipError;
};

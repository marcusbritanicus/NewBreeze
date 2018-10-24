/*
	*
	* LibBZip2.hpp - LibBZip2.cpp header
	*
*/

#pragma once

#include <QtCore>
#include <fstream>
#include <bzlib.h>

class NBBZip2 {

	public:
		NBBZip2( QString, QString file = QString() );
		bool extract();

		static QString fileName;
		static QString bz2FileName;

	private:
		BZFILE *bz2;
		FILE *bzFile;
		int NBBZip2Error;
};

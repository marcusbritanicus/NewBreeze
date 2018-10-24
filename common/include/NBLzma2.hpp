/*
	*
	* LibLzma.hpp - LibLzma.cpp header
	*
*/

#pragma once

#include <QtCore>
#include <assert.h>
#include <lzma.h>

class NBXz {

	public:
		NBXz( QString, QString file = QString() );
		bool extract();

		static QString fileName;
		static QString xzFileName;

	private:
		FILE *fdin, *fdout;
		int NBXzError;
};

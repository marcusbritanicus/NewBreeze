/*
	*
	* LibLzma.hpp - LibLzma.cpp header
	*
*/

#pragma once

#include <QtCore>
#include <assert.h>
#include <lzma.h>

class NBLzma {

	public:
		NBLzma( QString, QString file = QString() );
		bool extract();

		static QString fileName;
		static QString lzmaFileName;

	private:
		FILE *fdin, *fdout;
		int NBLzmaError;
};

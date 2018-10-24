/*
	*
	* LibBZip2.hpp - LibBZip2.cpp header
	*
*/

#pragma once

#include <QtCore>
#include <fstream>
#include <lzlib.h>

class NBLZip {

	public:
		NBLZip( QString, QString file = QString() );
		bool extract();

	private:
		QString fileName;
		QString lzFileName;

		int readblock( const int fd, uint8_t * const buf, const int size );
		int writeblock( const int fd, const uint8_t * const buf, const int size );
};

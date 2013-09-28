/*
	*
	* NBLibGZip.hpp - NBLibGZip.cpp header
	*
*/

#ifndef NBLIBGZIP_HPP
#define NBLIBGZIP_HPP

#include <Global.hpp>
#include <zlib.h>

class NBGZip {

	public:
		enum GZipError {
			NREG,			// Not a reguar file
			NFND,			// File/Folder not found
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBGZip( QString, NBGZip::Mode mode, QString file = QString() );
		void create();
		void extract();

		static QString fileName;
		static QString gzFileName;
		static NBGZip::Mode mode;

	private:
		gzFile gzip;
		int NBGZipError;
};

#endif

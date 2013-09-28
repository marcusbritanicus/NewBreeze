/*
	*
	* NBLibZip.hpp - NBLibZip.cpp header
	*
*/

#ifndef NBLIBZIP_HPP
#define NBLIBZIP_HPP

#include <Global.hpp>
#include <zip.h>

class NBZip {

	public:
		enum ZipError {
			NREG,			// Not a reguar file
			NFND,			// File/Folder not found
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBZip( QString, NBZip::Mode mode );
		void create( QStringList, QString );
		void extract( QString );

		static QString name;
		static NBZip::Mode mode;

	private:
		void extractMember( QString, QString );
		QStringList fileList;

		struct zip *zipArc;
		int NBZipError;
};

#endif

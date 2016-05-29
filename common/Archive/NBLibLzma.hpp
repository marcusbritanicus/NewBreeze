/*
	*
	* NBLibLzma.hpp - Legacy Lmza header
	*
*/

#pragma once

#include <QtCore>
#include <assert.h>
#include <lzma.h>

class NBLzma {

	public:
		enum LzmaError {
			NREG,			// Not a reguar file
			NFND,			// File/Folder not found
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBLzma( QString, NBLzma::Mode mode, QString file = QString() );
		void create();
		void extract();

		static QString fileName;
		static QString lzmaFileName;
		static NBLzma::Mode mode;

	private:
		FILE *fdin, *fdout;
		int NBLzmaError;
};

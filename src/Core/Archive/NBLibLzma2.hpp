/*
	*
	* NBLibLzma2.hpp - The new XZ class header
	*
*/

#pragma once

#include <QtCore>
#include <assert.h>
#include <lzma.h>

class NBXz {

	public:
		enum LzmaError {
			NREG,			// Not a reguar file
			NFND,			// File/Folder not found
		};

		enum Mode {
			READ,			// Read a zip file
			WRITE,			// Write a zip file
		};

		NBXz( QString, NBXz::Mode mode, QString file = QString() );
		void create();
		void extract();

		static QString fileName;
		static QString xzFileName;
		static NBXz::Mode mode;

	private:
		FILE *fdin, *fdout;
		int NBXzError;
};

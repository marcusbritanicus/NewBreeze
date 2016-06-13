/*
	*
	* NBLibLzma2.hpp - The new XZ class header
	*
*/

#pragma once

#include "common.hpp"
#include <lzma.h>

class NBCOMMON_DLLSPEC NBXz {

	public:
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

/*
	*
	* NBLibLzma.hpp - Legacy Lmza header
	*
*/

#pragma once

#include "common.hpp"
#include <lzma.h>

class NBCOMMON_DLLSPEC NBLzma {

	public:
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

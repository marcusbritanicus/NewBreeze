/*
	*
	* NBLibLzma.hpp - NBLibLzma.cpp header
	*
*/

#pragma once
#ifndef NBLIBLZMA_HPP
#define NBLIBLZMA_HPP

#include <Global.hpp>
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
		static QString xzFileName;
		static NBLzma::Mode mode;

	private:
		lzma_stream strm;
		lzma_ret ret;

		FILE *fdin, *fdout;
		int NBLzmaError;
};

#endif

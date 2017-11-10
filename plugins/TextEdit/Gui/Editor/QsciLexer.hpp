/*
	*
	* QsciLexer - QScintilla Lexers
	*
*/

#pragma once

#include "Global.hpp"
#include <Qsci/qsciscintilla.h>

#include <Qsci/qscilexercustom.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexertex.h>

class QLexerDefault : public QsciLexerCustom {
	Q_OBJECT

	public:
		QLexerDefault( QObject *parent = 0 );

		const char* language() const {

			return "Default";
		};

		QString description( int ) const {

			return QString();
		};

		void styleText( int, int ) {
		};
};

class QLexerPython : public QsciLexerPython {
	Q_OBJECT

	public:
		QLexerPython( QObject *parent = 0 );

		const char* keywords( int set ) const;

	private:
		char *keywords1;
		char *keywords2;

		void populateKeywords();
};

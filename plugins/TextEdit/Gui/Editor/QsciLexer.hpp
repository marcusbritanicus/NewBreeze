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
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexeroctave.h>

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

class QLexerJulia : public QsciLexerOctave {
	Q_OBJECT

	public:
		enum{
			Default = 0,
			Comment = 1,
			Number = 2,
			DoubleQuotedString = 3,
			SingleQuotedString = 4,
			Keyword = 5,
			ClassName = 8,
			FunctionMethodName = 9,
			Operator = 10,
			Identifier = 11,
			CommentBlock = 12,
			UnclosedString = 13,
			HighlightedIdentifier = 14,
			Decorator = 15,
			DoubleQuotedFString = 16,
			SingleQuotedFString = 17,
		};

		QLexerJulia( QObject *parent = 0 );

		const char* language() const {

			return "Julia";
		};

		const char* keywords( int set ) const;

	private:
		void populateKeywords();

		static QString keywords1;
		static QString keywords2;
};

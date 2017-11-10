/*
	*
	* QsciLexerLaTeX - LaTeX Lexer for QScintilla
	*
*/

#pragma once

#include "Global.hpp"
#include <Qsci/qscilexercustom.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscistyle.h>

class QsciLexerLaTeX : public QsciLexerCustom {

	public:
		enum{
			Default = 0,
			Comment = 1,
			Command = 2,
			MathInline = 3,
			MathDisplay = 4,
			SpecialChar = 5,
			Option = 6,
			Digit = 7,
			Environment = 8,
			Part = 9,
			Chapter = 10,
			Section = 11,
			SubSection = 12,
			SubSubSection = 13,
			Paragraph = 14
		};

		QsciLexerLaTeX( QObject *parent = 0 );

		const char* language() const;
		QString description( int ) const;
		void styleText( int, int );

		QColor defaultColor( int ) const;
		QFont defaultFont( int ) const;
		QColor defaultPaper( int ) const;
		bool defaultEolFill( int ) const;

		QsciStyle defaultStyle( int ) const;

		const char* wordCharacters() const;

	private:
		QStringList tokenize( QString );
};

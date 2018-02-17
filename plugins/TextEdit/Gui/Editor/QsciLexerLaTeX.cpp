/*
	*
	* QsciLexerLaTeX - LaTeX Lexer for QScintilla
	*
*/

#include "QsciLexerLaTeX.hpp"
#include <Qsci/qsciscintilla.h>

QsciLexerLaTeX::QsciLexerLaTeX( QObject *parent ) : QsciLexerCustom( parent ) {

	setDefaultFont( QFont( "Envy Code R", 9 ) );

	setDefaultPaper( QColor( "#000000" ) );

	setFont( QFont( "Envy Code R", 9 ), -1 );

	setColor( QColor( Qt::white ), Default );
	setColor( QColor( "#55AA00" ), Comment );
	setColor( QColor( "#008080" ), Command );
	setColor( QColor( "#800000" ), MathInline );
	setColor( QColor( "#800000" ), MathDisplay );
	setColor( QColor( "#800000" ), SpecialChar );
	setColor( QColor( "#004040" ), Option );
	setColor( QColor( "#400000" ), Digit );
	setColor( QColor( "#FF8000" ), Environment );
	setColor( QColor( "#AACCFF" ), Part );
	setColor( QColor( "#AACCFF" ), Chapter );
	setColor( QColor( "#AACCFF" ), Section );
	setColor( QColor( "#AACCFF" ), SubSection );
	setColor( QColor( "#AACCFF" ), SubSubSection );
	setColor( QColor( "#AACCFF" ), Paragraph );
};

const char* QsciLexerLaTeX::language() const {

	return "LaTeX";
};

QString QsciLexerLaTeX::description( int style ) const {

	switch ( style ) {
		case Default:
			return "Default";
			break;

		case Comment:
			return "Comment";
			break;

		case Command:
			return "Command";
			break;

		case MathInline:
			return "MathInline";
			break;

		case MathDisplay:
			return "MathDisplay";
			break;

		case SpecialChar:
			return "SpecialChar";
			break;

		case Option:
			return "Option";
			break;

		case Digit:
			return "Digit";
			break;

		case Environment:
			return "Environment";
			break;

		case Part:
			return "Part";
			break;

		case Chapter:
			return "Chapter";
			break;

		case Section:
			return "Section";
			break;

		case SubSection:
			return "SubSection";
			break;

		case SubSubSection:
			return "SubSubSection";
			break;

		case Paragraph:
			return "Paragraph";
			break;

		default:
			return "";
			break;
	}
};

void QsciLexerLaTeX::styleText( int start, int end ) {

	if ( not editor() )
		return;

	QString lines = editor()->text().mid( start, end - start + 1 );
	startStyling( start );

	int style = Default;

	QStringList tokens = tokenize( lines );
	Q_FOREACH( QString token, tokens ) {
		if ( token == "\\" )
			style = Command;

		else if ( token == "%" )
			style = Comment;

		else if ( token == "[" )
			style = Option;

		else if ( token == "]" ) {
			setStyling( token.size(), style );
			style = Default;
			continue;
		}

		else if ( token == "$" and style == MathInline ) {
			setStyling( token.size(), style );
			style = Default;
			continue;
		}

		else if ( token == "$" and style != MathInline )
			style = MathInline;

		else if ( token == "{" ) {
			if ( style != Option )
				style = Default;
		}

		else if ( token == "}" ) {
			if ( style == Option ) {
				setStyling( token.size(), style );
				style = Default;
				continue;
			}

			style = Default;
		}

		else if ( token == "\r" or token == "\n" )
			style = Default;

		setStyling( token.size(), style );
	}
};

QColor QsciLexerLaTeX::defaultColor( int style ) const {

	switch( style ) {

		case Default:
			return QColor( Qt::black );
			break;

		case Comment:
			return QColor( "#55AA00" );
			break;

		case Command:
			return QColor( "#008080" );
			break;

		case MathInline:
			return QColor( "#800000" );
			break;

		case MathDisplay:
			return QColor( "#800000" );
			break;

		case SpecialChar:
			return QColor( "#800000" );
			break;

		case Option:
			return QColor( "#400000" );
			break;

		case Digit:
			return QColor( "#800000" );
			break;

		case Environment:
			return QColor( "FF8000" );
			break;

		case Part:
		case Chapter:
		case Section:
		case SubSection:
		case SubSubSection:
		case Paragraph:
			return QColor( "#AACCFF" );
			break;

		default:
			return QsciLexerCustom::defaultColor();
			break;
	}
};

QFont QsciLexerLaTeX::defaultFont( int style ) const {

	switch( style ) {
		case Default:
			return QFont( "Envy Code R", 9 );
			break;

		case Comment:
			return QFont( "Envy Code R", 9, QFont::Normal, true );
			break;

		case Command:
			return QFont( "Envy Code R", 9 );
			break;

		case MathInline:
			return QFont( "Envy Code R", 9 );
			break;

		case MathDisplay:
			return QFont( "Envy Code R", 9 );
			break;

		case SpecialChar:
			return QFont( "Envy Code R", 9 );
			break;

		case Option:
			return QFont( "Envy Code R", 9 );
			break;

		case Digit:
			return QFont( "Envy Code R", 9 );
			break;

		case Environment:
			return QFont( "Envy Code R", 9, QFont::Bold );
			break;

		case Part:
			return QFont( "Envy Code R", 9, QFont::Bold );
			break;

		case Chapter:
			return QFont( "Envy Code R", 9, QFont::Bold );
			break;

		case Section:
			return QFont( "Envy Code R", 9, QFont::Bold );
			break;

		case SubSection:
			return QFont( "Envy Code R", 9, QFont::Bold, true );
			break;

		case SubSubSection:
			return QFont( "Envy Code R", 9, QFont::Normal );
			break;

		case Paragraph:
			return QFont( "Envy Code R", 9, QFont::Normal, true );
			break;

		default:
			return QsciLexerCustom::defaultFont();
			break;
	}
};

QColor QsciLexerLaTeX::defaultPaper( int style ) const {

	return QsciLexerCustom::defaultPaper( style );
};

bool QsciLexerLaTeX::defaultEolFill( int style ) const {

	return QsciLexerCustom::defaultEolFill( style );
};

QsciStyle QsciLexerLaTeX::defaultStyle( int style ) const {

	return QsciStyle( style, description( style ), defaultColor( style ), defaultPaper( style ), defaultFont( style ), defaultEolFill( style ) );
};

const char* QsciLexerLaTeX::wordCharacters() const{

	return "abcdefghijklmnopqrstuvwxyz\\@";
};

QStringList QsciLexerLaTeX::tokenize( QString text ) {

	/* QStringList to hold the tokens */
	QStringList tokens;
	QString token;

	Q_FOREACH( QChar ch, text ) {
		if ( not ( ch.isLetter() or ch.isDigit() ) ) {
			if ( token.size() ) {
				tokens << token;
				token = QString();
			}

			tokens << QString( ch );
		}

		else if ( token.size() ) {
			if ( token.at( 0 ).isLetter() and QChar( ch ).isLetter() ) {

				token += ch;
			}

			else if ( token.at( 0 ).isDigit() and QChar( ch ).isDigit() ) {

				token += ch;
			}

			else {
				tokens << token;
				token = ch;
			}
		}

		else {

			token = ch;
		}
	}

	return tokens;
};

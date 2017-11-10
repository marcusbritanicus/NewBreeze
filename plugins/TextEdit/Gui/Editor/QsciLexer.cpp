/*
	*
	* QsciLexer - Lexers for QScintilla
	*
*/

#include "QsciLexer.hpp"
#include <QDebug>

QLexerDefault::QLexerDefault( QObject *parent ) : QsciLexerCustom( parent ) {

	setDefaultFont( QFont( "Envy Code R", 9 ) );

	setDefaultPaper( QColor( "#000000" ) );
	setColor( QColor( "#FFFFFF" ), 0 );

	setFont( QFont( "Envy Code R", 9 ), -1 );
};

QLexerPython::QLexerPython( QObject *parent ) : QsciLexerPython( parent ) {

	populateKeywords();

	setDefaultFont( QFont( "Envy Code R", 9 ) );

	setDefaultPaper( QColor( "#000000" ) );
	setColor( QColor( "#FFFFFF" ), QsciLexerPython::Default );
	setColor( QColor( "#AAAAFF" ), QsciLexerPython::ClassName );
	setColor( QColor( "#55AA00" ), QsciLexerPython::Comment );
	setColor( QColor( "#55AA00" ), QsciLexerPython::CommentBlock );
	setColor( QColor( "#77DDFF" ), QsciLexerPython::Decorator );
	setColor( QColor( "#FFFF00" ), QsciLexerPython::DoubleQuotedString );
	setColor( QColor( "#FF00FF" ), QsciLexerPython::FunctionMethodName );
	setColor( QColor( "#FFAAFF" ), QsciLexerPython::HighlightedIdentifier );
	setColor( QColor( "#FFFFFF" ), QsciLexerPython::Identifier );
	setColor( QColor( "#8080FF" ), QsciLexerPython::Keyword );
	setColor( QColor( "#AC3C90" ), QsciLexerPython::Number );
	setColor( QColor( "#FABBAF" ), QsciLexerPython::Operator );
	setColor( QColor( "#FFFF00" ), QsciLexerPython::SingleQuotedString );
	setColor( QColor( "#FF0000" ), QsciLexerPython::UnclosedString );
	setColor( QColor( "#FFFF00" ), QsciLexerPython::TripleDoubleQuotedString );
	setColor( QColor( "#FFFF00" ), QsciLexerPython::TripleSingleQuotedString );

	setFont( QFont( "Envy Code R", 9 ), -1 );

	setFoldComments( true );
	setFoldQuotes( true );
};

const char* QLexerPython::keywords( int set ) const {

	if ( set == 1 )
		return keywords1;

	else if ( set == 2 )
		return keywords2;

	else
		return 0;
};

void QLexerPython::populateKeywords() {

	QSettings pylex( "NewBreeze", "Lexers/Python" );
	QString kwd1 = pylex.value( "Keywords1" ).toString();
	QString kwd2 = pylex.value( "Keywords2" ).toString();

	if ( kwd1.isEmpty() ) {
		pylex.setValue( "Keywords1", QsciLexerPython::keywords( 1 ) );
		kwd1 = QString::fromLocal8Bit( QsciLexerPython::keywords( 1 ) );
		pylex.sync();
	}

	if ( kwd2.isEmpty() ) {
		pylex.setValue( "Keywords2", "None True False self hasattr getattr bool open file float int str long complex set hex oct" );
		kwd2 = QString::fromLocal8Bit( "None True False self hasattr getattr bool open file float int str long complex set hex oct" );
		pylex.sync();
	}

	keywords1 = new char[ kwd1.size() + 1 ];
	strcpy( keywords1, kwd1.toLocal8Bit().constData() );

	keywords2 = new char[ kwd2.size() + 1 ];
	strcpy( keywords2, kwd2.toLocal8Bit().constData() );
};

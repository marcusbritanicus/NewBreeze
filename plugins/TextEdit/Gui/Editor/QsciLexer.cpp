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

QString QLexerJulia::keywords1 = QString();
QString QLexerJulia::keywords2 = QString();

QLexerJulia::QLexerJulia( QObject *parent ) : QsciLexerOctave( parent ) {

	populateKeywords();

	setDefaultFont( QFont( "Envy Code R", 9 ) );

	setDefaultPaper( QColor( "#000000" ) );
	setColor( QColor( "#FFFFFF" ), QLexerJulia::Default );
	setColor( QColor( "#AAAAFF" ), QLexerJulia::ClassName );
	setColor( QColor( "#55AA00" ), QLexerJulia::Comment );
	setColor( QColor( "#55AA00" ), QLexerJulia::CommentBlock );
	setColor( QColor( "#77DDFF" ), QLexerJulia::Decorator );
	setColor( QColor( "#FFFF00" ), QLexerJulia::DoubleQuotedString );
	setColor( QColor( "#FF00FF" ), QLexerJulia::FunctionMethodName );
	setColor( QColor( "#FFAAFF" ), QLexerJulia::HighlightedIdentifier );
	setColor( QColor( "#FFFFFF" ), QLexerJulia::Identifier );
	setColor( QColor( "#8080FF" ), QLexerJulia::Keyword );
	setColor( QColor( "#AC3C90" ), QLexerJulia::Number );
	setColor( QColor( "#FABBAF" ), QLexerJulia::Operator );
	setColor( QColor( "#FFFF00" ), QLexerJulia::SingleQuotedString );
	setColor( QColor( "#FF0000" ), QLexerJulia::UnclosedString );

	setFont( QFont( "Envy Code R", 9 ), -1 );

	// setFoldComments( true );
	// setFoldQuotes( true );
};

const char* QLexerJulia::keywords( int set ) const {

	if ( set == 1 )
		return keywords1.toLocal8Bit().constData();

	else if ( set == 2 )
		return keywords2.toLocal8Bit().constData();

	else
		return 0;
};

void QLexerJulia::populateKeywords() {

	QString keywords1 = QString(
		"function global for end while if else elseif break switch case otherwise try catch end const immutable import importall export type typealias return true false "
		"macro quote in abstract module using continue ifelse do eval let finally throw"
	);

	QString keywords2 = QString(
		"Array String Bool Number Int Integer Real Complex FloatingPoint Float64 Float32 Int8 Int16 Int32 Int64 Rational AbstractArray Unsigned Signed Uint Uint8 "
		"Uint16 Uint32 Uint64 Vector AbstractVector Matrix AbstractMatrix Type IO Any ASCIIString Union Dict Function SubArray Range Range1 Symbol Expr cell "
		"collect filter merge divrem hex dec oct base int round cmp float linspace fill   start done tuple minimum maximum count index append push pop shift "
		"unshift insert splice reverse sort zip length delete copy haskey keys values get getkey Set isa issubset intersect setdiff symdiff complement print printf "
		"println sprintf join utf8 char search rsearch beginswith endswith replace lowercase uppercase ucfirst lcfirst split rsplit chop chomp lpad rpad lstrip "
		"rstrip strip isalnum isalpha isascii isblank iscntrl isdigit isgraph islower isprint ispunct isspace isupper isxdigit match captures offset offsets "
		"matchall eachmatch hcat vcat hvcat reshape deepcopy similar reinterpret map reduce mapreduce DataArray DataFrame removeNA replaceNA colnames head tail "
		"describe join groupby by stack readtable readcsv readdlm writetable writecsv writedlm require reload include evalfile cd open write close position seek "
		"seekstart seekend skip isopen eof isreadonly ltoh htol serialize deserialize download run spawn success process_running process_exited kill readsfrom "
		"writesto readsandwrite detach setenv ENV getpid clipboard strftime time cd gethostname getipaddr pwd mkdir mkpath rmdir ignorestatus "
	);
};

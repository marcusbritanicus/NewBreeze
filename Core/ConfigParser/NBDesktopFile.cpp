/*
	*
	* NBDesktopFile.cpp - NewBreeze DesktopFile Handler Class
	*
*/

#include <NBDesktopFile.hpp>

inline QString findIn( QString what, QString where ) {

	QRegExp rx( what );
	rx.setMinimal( true );

	if ( rx.indexIn( where ) >= 0 )
		return rx.cap( 1 );

	return QString();
};

NBDesktopFile::NBDesktopFile( QString path ) {

	fileUrl = QString( path );
	parseDesktopFile();
};

QString NBDesktopFile::name() {

	return __name;
};

QString NBDesktopFile::type() {

	return __type;
};

QString NBDesktopFile::exec() {

	return __exec;
};

QString NBDesktopFile::icon() {

	return __icon;
};

QStringList NBDesktopFile::mimeTypes() {

	return __mimeTypes;
};

QString NBDesktopFile::workPath() {

	return __workPath;
};

bool NBDesktopFile::terminalMode() {

	return __terminalMode;
};

QStringList NBDesktopFile::categories() {

	return __categories;
};

QString NBDesktopFile::comment() {

	return __comment;
};

QStringList NBDesktopFile::execArgs() {

	return __execArgs;
};

bool NBDesktopFile::multipleArgs() {

	return __multipleFiles;
};

bool NBDesktopFile::takesArgs() {

	return __takesArgs;
};

short NBDesktopFile::grade() {

	return __grade;
};

QString NBDesktopFile::desktopFileName() {

	return baseName( fileUrl );
};

bool NBDesktopFile::isSameAs( NBDesktopFile other ) {

	if ( __execArgs.at( 0 ) == other.execArgs().at( 0 ) )
		return true;

	return false;
};

bool NBDesktopFile::areSame( NBDesktopFile dEntry1, NBDesktopFile dEntry2 ) {

	if ( dEntry1.execArgs().at( 0 ) == dEntry2.execArgs().at( 0 ) )
		return true;

	return false;
};

void NBDesktopFile::parseDesktopFile() {

	QString rxName( "\nName=(.*)(\n|\r\n)" );
	QString rxType( "\nType=(.*)(\n|\r\n)" );
	QString rxExec( "\nExec=(.*)(\n|\r\n)" );
	QString rxIcon( "\nIcon=(.*)(\n|\r\n)" );
	QString rxPath( "\nPath=(.*)(\n|\r\n)" );
	QString rxMime( "\nMimeType=(.*)(\n|\r\n)" );
	QString rxTerm( "\nTerminal=(.*)(\n|\r\n)" );
	QString rxCate( "\nCategories=(.*)(\n|\r\n)" );
	QString rxComm( "\nComment=(.*)(\n|\r\n)" );

	QFile dFile( fileUrl );
	if ( !dFile.exists() )
		return;

	if ( !dFile.open( QFile::ReadOnly ) )
		return;

	QString entry = QString( dFile.readAll() );
	dFile.close();

	__name = findIn( rxName, entry );

	if ( __name.isEmpty() )
		qDebug() << "Nameless monster:" << fileUrl;

	__type = findIn( rxType, entry );
	__exec = findIn( rxExec, entry );

	__icon = findIn( rxIcon, entry );
	__workPath = findIn( rxPath, entry );
	__mimeTypes = findIn( rxMime, entry ).split( ";", QString::SkipEmptyParts );
	QString __terminal = findIn( rxTerm, entry );
	__terminalMode = ( __terminal.toLower() == "true" ? true : false );

	__categories << findIn( rxCate, entry ).split( ";", QString::SkipEmptyParts );
	__comment = findIn( rxComm, entry );

	// By default set @v __multipleFiles to false
	__multipleFiles = false;
	__takesArgs = false;

	QStringList args = __exec.split( " " );
	foreach( QString arg, args ) {
		if ( arg == "%f" or arg == "%u" ) {
			__multipleFiles = false;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILE#>";
		}

		else if ( arg == "%F" or arg == "%U" ) {
			__multipleFiles = true;
			__takesArgs = true;
			__execArgs << "<#NEWBREEZE-ARG-FILES#>";
		}

		else if ( arg == "%i" ) {
			if ( !__icon.isEmpty() )
				__execArgs << "--icon" << __icon;
		}

		else if ( arg == "%c" )
			__execArgs << __name;

		else if ( arg == "%k" )
			__execArgs << QUrl( fileUrl ).toLocalFile();

		else
			__execArgs << arg;
	}
};

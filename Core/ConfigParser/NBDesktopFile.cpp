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

void NBDesktopFile::parseDesktopFile() {

	QString rxName( "\nName=(.*)(\n|\r\n)" );
	QString rxType( "\nType=(.*)(\n|\r\n)" );
	QString rxExec( "\nExec=(.*)(\n|\r\n)" );
	QString rxIcon( "\nIcon=(.*)(\n|\r\n)" );
	QString rxPath( "\nPath=(.*)(\n|\r\n)" );
	QString rxMime( "\nMimeType=(.*)(\n|\r\n)" );
	QString rxTerm( "\nTerminal=(.*)(\n|\r\n)" );

	QFile dFile( fileUrl );
	if ( !dFile.exists() )
		return;

	if ( !dFile.open( QFile::ReadOnly ) )
		return;

	QString entry = QString( dFile.readAll() );
	dFile.close();

	name = findIn( rxName, entry );

	if ( name.isEmpty() )
		qDebug() << fileUrl;

	type = findIn( rxType, entry );
	exec = findIn( rxExec, entry );

	icon = findIn( rxIcon, entry );
	path = findIn( rxPath, entry );
	mimetypes = findIn( rxMime, entry ).split( ";", QString::SkipEmptyParts );
	terminal = findIn( rxTerm, entry );

	generateExecArgs();
};

void NBDesktopFile::getMimeTypeList() {

	QFile dFile( fileUrl );
	dFile.open( QFile::ReadOnly );
	QStringList lines = QString( dFile.readAll() ).split( "\n", QString::SkipEmptyParts );
	dFile.close();

	foreach( QString line, lines ) {
		if ( line.startsWith( "MimeType" ) ) {
			mimetypes = line.split( "=" )[ 1 ].split( ";", QString::SkipEmptyParts );
			break;
		}
	}

};

void NBDesktopFile::generateExecArgs() {

	// By default set @v multipleInputFiles to false
	multipleInputFiles = false;
	canTakeArgs = false;

	QStringList args = exec.split( " " );
	foreach( QString arg, args ) {
		if ( arg == "%f" or arg == "%u" ) {
			multipleInputFiles = false;
			canTakeArgs = true;
			execArgs << "<#NEWBREEZE-ARG-FILE#>";
		}

		else if ( arg == "%F" or arg == "%U" ) {
			multipleInputFiles = true;
			canTakeArgs = true;
			execArgs << "<#NEWBREEZE-ARG-FILES#>";
		}

		else if ( arg == "%i" ) {
			if ( !icon.isEmpty() )
				execArgs << "--icon" << icon;
		}

		else if ( arg == "%c" )
			execArgs << name;

		else if ( arg == "%k" )
			execArgs << QUrl( fileUrl ).toLocalFile();

		else
			execArgs << arg;
	}
};

bool NBDesktopFile::takesArgs() {

	return canTakeArgs;
};

bool NBDesktopFile::sameProgram( QString desktopEntry ) {

	NBDesktopFile dEntry( desktopEntry );

	if ( execArgs[ 0 ] == dEntry.execArgs[ 0 ] )
		return true;

	return false;
};
bool NBDesktopFile::sameProgram( QString desktopEntry1, QString desktopEntry2 ) {

	NBDesktopFile dEntry1( desktopEntry1 );
	NBDesktopFile dEntry2( desktopEntry2 );

	if ( dEntry1.execArgs[ 0 ] == dEntry2.execArgs[ 0 ] )
		return true;

	return false;
};

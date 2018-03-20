/*
	*
	* Copyright 2012 Britanicus <marcusbritanicus@gmail.com>
	*
*/

#include "NBTTools.hpp"
#include <gnu/libc-version.h>
#include <sys/utsname.h>
#include <getopt.h>

QString getStyleSheet( QString Widget ) {

	QFile file( qApp->tr( ":/StyleSheets/%1.qss" ).arg( Widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	return QSS;
}

const char* platform() {

	struct utsname buffer;
	uname( &buffer );

	QString Qt4Version = QString( "Powered by Qt " ) + QT_VERSION_STR + " on Linux " + buffer.release;
	QString GCCVersion = QString( "Compiled with GCC " ) + __VERSION__;

	QString version = Qt4Version + '\n' + GCCVersion;

	return qPrintable( version );
}

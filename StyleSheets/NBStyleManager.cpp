/*
	*
	* NBStyleManager.cpp - NBStyleManager Class
	*
*/

#include <NBStyleManager.hpp>

NBStyleManager::NBStyleManager() {

	// Item: TransDark, TransLight, LightGray, DullBlack
	variableMap[ "<bg>" ] = QStringList() << QString( "bg" ) << QString( "bg2" ) << QString( "" ) << QString( "bg" );
	variableMap[ "<bgcolor>" ] = QStringList() << QString( "transparent" ) << QString( "transparent" ) << QString( "lightgray" ) << QString( "transparent" );
	variableMap[ "<textcolor>" ] = QStringList() << QString( "white" ) << QString( "black" ) << QString( "black" ) << QString( "white" );
	variableMap[ "<81>" ] = QStringList() << QString( "0, 0, 0, 206" ) << QString( "255, 255, 255, 206" ) << QString( "255, 255, 255, 255" ) << QString( "0, 0, 0, 255" );
	variableMap[ "<72>" ] = QStringList() << QString( "0, 0, 0, 183" ) << QString( "255, 255, 255, 183" ) << QString( "255, 255, 255, 255" ) << QString( "0, 0, 0, 255" );
	variableMap[ "<27>" ] = QStringList() << QString( "0, 0, 0,  68" ) << QString( "255, 255, 255,  68" ) << QString( "255, 255, 255, 255" ) << QString( "0, 0, 0, 255" );
	variableMap[ "<bordercolor>" ] = QStringList() << QString( "white" ) << QString( "black" ) << QString( "black" ) << QString( "white" );
	variableMap[ "<disabled>" ] = QStringList() << QString( "lightgray" ) << QString( "gray" ) << QString( "gray" ) << QString( "lightgray" );
	variableMap[ "<checked>" ] = QStringList() << QString( "checked" ) << QString( "checkedn" ) << QString( "checkedn" ) << QString( "checked" );
	variableMap[ "<unchecked>" ] = QStringList() << QString( "unchecked" ) << QString( "uncheckedn" ) << QString( "uncheckedn" ) << QString( "unchecked" );
	variableMap[ "<scrollcolor>" ] = QStringList() << QString( "silver" ) << QString( "gray" ) << QString( "#2F2F2F" ) << QString( "silver" );
};

QString NBStyleManager::getStyleSheet( QString widget, QString style ) {

	if ( style == QString( "TransDark" ) )
		return getStyleSheetTD( widget );

	else if ( style == QString( "TransLight" ) )
		return getStyleSheetTL( widget );

	else if ( style == QString( "LightGray" ) )
		return getStyleSheetLG( widget );

	else // DullBlack
		return getStyleSheetDB( widget );
};

QString NBStyleManager::getStyleSheetTD( QString widget ) {

	QFile file( qApp->tr( ":/StyleSheets/Template/%1.qss" ).arg( widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	foreach( QString key, variableMap.keys() )
		QSS.replace( key, variableMap[ key ].at( 0 ) );

	return QSS;
};

QString NBStyleManager::getStyleSheetTL( QString widget ) {

	QFile file( qApp->tr( ":/StyleSheets/Template/%1.qss" ).arg( widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	foreach( QString key, variableMap.keys() )
		QSS.replace( key, variableMap[ key ].at( 1 ) );

	return QSS;
};

QString NBStyleManager::getStyleSheetLG( QString widget ) {

	QFile file( qApp->tr( ":/StyleSheets/Template/%1.qss" ).arg( widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	foreach( QString key, variableMap.keys() )
		QSS.replace( key, variableMap[ key ].at( 2 ) );

	return QSS;
};

QString NBStyleManager::getStyleSheetDB( QString widget ) {

	QFile file( qApp->tr( ":/StyleSheets/Template/%1.qss" ).arg( widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	foreach( QString key, variableMap.keys() )
		QSS.replace( key, variableMap[ key ].at( 3 ) );

	return QSS;
};

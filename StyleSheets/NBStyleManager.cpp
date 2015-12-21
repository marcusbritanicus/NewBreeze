/*
	*
	* NBStyleManager.cpp - NBStyleManager Class
	*
*/

#include <NBStyleManager.hpp>

bool NBStyleManager::initDone = false;
QMap<QString, QStringList> NBStyleManager::variableMap = QMap<QString, QStringList>();

void NBStyleManager::initStyleManager() {

	// Item: TransDark, TransLight, LightGray, DullBlack
	variableMap[ "<textcolor>" ] = QStringList() << QString( "white" ) << QString( "black" ) << QString( "black" ) << QString( "white" );
	variableMap[ "<81>" ] = QStringList() << QString( "0, 0, 0, 206" ) << QString( "255, 255, 255, 206" ) << QString( "255, 255, 255, 255" ) << QString( "0, 0, 0, 255" );
	variableMap[ "<bordercolor>" ] = QStringList() << QString( "white" ) << QString( "black" ) << QString( "black" ) << QString( "white" );
	variableMap[ "<disabled>" ] = QStringList() << QString( "lightgray" ) << QString( "gray" ) << QString( "gray" ) << QString( "lightgray" );
	variableMap[ "<scrollcolor>" ] = QStringList() << QString( "silver" ) << QString( "gray" ) << QString( "#2F2F2F" ) << QString( "silver" );

	initDone = true;
};

QString NBStyleManager::getStyleSheet( QString widget, QString style ) {

	if ( not initDone )
		initStyleManager();

	if ( style == QString( "TransDark" ) )
		return getStyleSheetTD( widget );

	else if ( style == QString( "TransLight" ) )
		return getStyleSheetTL( widget );

	else if ( style == QString( "LightGray" ) )
		return getStyleSheetLG( widget );

	else // DullBlack
		return getStyleSheetDB( widget );
};

QPalette NBStyleManager::getPalette( QString ) {

	return qApp->palette();
};

QPalette NBStyleManager::transparentPalette() {

	return qApp->palette();
};

QString NBStyleManager::getStyleSheetTD( QString widget ) {

	if ( not initDone )
		initStyleManager();

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

	if ( not initDone )
		initStyleManager();

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

	if ( not initDone )
		initStyleManager();

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

	if ( not initDone )
		initStyleManager();

	QFile file( qApp->tr( ":/StyleSheets/Template/%1.qss" ).arg( widget ) );
	if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		return QString();

	QString QSS = QString( file.readAll() );
	file.close();

	foreach( QString key, variableMap.keys() )
		QSS.replace( key, variableMap[ key ].at( 3 ) );

	return QSS;
};

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

QPalette NBStyleManager::getPalette( QString style ) {

	if ( style == QString( "TransDark" ) )
		return getPaletteTD();

	else if ( style == QString( "TransLight" ) )
		return getPaletteTL();

	else if ( style == QString( "LightGray" ) )
		return getPaletteLG();

	else // DullBlack
		return getPaletteDB();
};

QPalette NBStyleManager::transparentPalette() {

	QPalette pltt = qApp->palette();

	pltt.setColor( QPalette::Active, QPalette::Window, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Active, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::AlternateBase, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Active, QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Active, QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Active, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Active, QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Active, QPalette::Link, Qt::darkGreen );

	pltt.setColor( QPalette::Disabled, QPalette::Window, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::AlternateBase, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Disabled, QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Link, Qt::darkGreen );

	pltt.setColor( QPalette::Inactive, QPalette::Window, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::AlternateBase, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Inactive, QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Link, Qt::darkGreen );

	return pltt;
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

QPalette NBStyleManager::getPaletteTD() {

	QPalette pltt = qApp->palette();

	pltt.setColor( QPalette::Active,   QPalette::Window, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Active,   QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Active,   QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Active,   QPalette::AlternateBase, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Active,   QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Active,   QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Active,   QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Active,   QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Active,   QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Active,   QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Active,   QPalette::Link, Qt::darkGreen );

	pltt.setColor( QPalette::Disabled, QPalette::Window, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Disabled, QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::AlternateBase, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Disabled, QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Disabled, QPalette::Link, Qt::darkGreen );

	pltt.setColor( QPalette::Inactive, QPalette::Window, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Inactive, QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::AlternateBase, QColor( 30, 30, 30, 180 ) );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipBase, QColor( 0, 0, 0, 206 ) );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Inactive, QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::BrightText, Qt::white );
	pltt.setColor( QPalette::Inactive, QPalette::Link, Qt::darkGreen );

	return pltt;
};

QPalette NBStyleManager::getPaletteTL() {

	QPalette pltt = qApp->palette();

	pltt.setColor( QPalette::Active, QPalette::Window, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Active, QPalette::WindowText, Qt::black );
	pltt.setColor( QPalette::Active, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::AlternateBase, QColor( 90, 90, 90, 180 ) );
	pltt.setColor( QPalette::Active, QPalette::ToolTipBase, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Active, QPalette::ToolTipText, QColor( 0x00, 0x43, 0x8A ) );
	pltt.setColor( QPalette::Active, QPalette::Text, Qt::black );
	pltt.setColor( QPalette::Active, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Active, QPalette::ButtonText, Qt::black );
	pltt.setColor( QPalette::Active, QPalette::BrightText, Qt::black );

	pltt.setColor( QPalette::Disabled, QPalette::Window, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Disabled, QPalette::WindowText, Qt::black );
	pltt.setColor( QPalette::Disabled, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::AlternateBase, QColor( 90, 90, 90, 180 ) );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipBase, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Disabled, QPalette::ToolTipText, QColor( 0x00, 0x43, 0x8A ) );
	pltt.setColor( QPalette::Disabled, QPalette::Text, Qt::black );
	pltt.setColor( QPalette::Disabled, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Disabled, QPalette::ButtonText, Qt::black );
	pltt.setColor( QPalette::Disabled, QPalette::BrightText, Qt::black );

	pltt.setColor( QPalette::Inactive, QPalette::Window, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Inactive, QPalette::WindowText, Qt::black );
	pltt.setColor( QPalette::Inactive, QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::AlternateBase, QColor( 90, 90, 90, 180 ) );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipBase, QColor( 120, 120, 120, 206 ) );
	pltt.setColor( QPalette::Inactive, QPalette::ToolTipText, QColor( 0x00, 0x43, 0x8A ) );
	pltt.setColor( QPalette::Inactive, QPalette::Text, Qt::black );
	pltt.setColor( QPalette::Inactive, QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::Inactive, QPalette::ButtonText, Qt::black );
	pltt.setColor( QPalette::Inactive, QPalette::BrightText, Qt::black );

	return pltt;
};

QPalette NBStyleManager::getPaletteLG() {

	QPalette pltt = qApp->palette();

	pltt.setColor( QPalette::Window, QColor( 120, 120, 120 ) );
	pltt.setColor( QPalette::WindowText, Qt::black );
	pltt.setColor( QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::AlternateBase, QColor( 90, 90, 90 ) );
	pltt.setColor( QPalette::ToolTipBase, QColor( 100, 100, 100 ) );
	pltt.setColor( QPalette::ToolTipText, QColor( 0x00, 0x43, 0x8A ) );
	pltt.setColor( QPalette::Text, Qt::black );
	pltt.setColor( QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::ButtonText, Qt::black );
	pltt.setColor( QPalette::BrightText, Qt::black );

	return pltt;
};

QPalette NBStyleManager::getPaletteDB() {

	QPalette pltt = qApp->palette();

	pltt.setColor( QPalette::Window, QColor( 30, 30, 30 ) );
	pltt.setColor( QPalette::WindowText, Qt::white );
	pltt.setColor( QPalette::Base, Qt::transparent );
	pltt.setColor( QPalette::AlternateBase, QColor( 60, 60, 60) );
	pltt.setColor( QPalette::ToolTipBase, QColor( 45, 45, 45 ) );
	pltt.setColor( QPalette::ToolTipText, QColor( 0x61, 0x93, 0xCF ) );
	pltt.setColor( QPalette::Text, Qt::white );
	pltt.setColor( QPalette::Button, Qt::transparent );
	pltt.setColor( QPalette::ButtonText, Qt::white );
	pltt.setColor( QPalette::BrightText, Qt::white );

	return pltt;
};

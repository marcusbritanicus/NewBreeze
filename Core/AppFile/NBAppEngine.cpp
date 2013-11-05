/*
	*
	* NBAppEngine.cpp - NewBreeze Application Engine
	*
*/

#include <NBAppEngine.hpp>

inline QString findIn( QString what, QString where ) {

	QRegExp rx( what );
	rx.setMinimal( true );

	if ( rx.indexIn( where ) >= 0 )
		return rx.cap( 1 );

	return QString();
};

NBAppEngine *NBAppEngine::globalInstance = NULL;

NBAppsList NBAppEngine::appsForMimeType( QMimeType mimeType ) {

	NBAppsList appsForMimeList;

	QStringList mimeList = QStringList() << mimeType.name() << mimeType.allAncestors();

	foreach( NBAppFile app, appsList.toQList() ) {
		foreach( QString mime, mimeList ) {
			if ( app.value( NBAppFile::MimeTypes ).toStringList().contains( mime ) )
				if ( ( app.value( NBAppFile::Type ).toString() == "Application" ) and ( not app.value( NBAppFile::NoDisplay ).toBool() ) )
					if ( not appsForMimeList.contains( app ) )
						appsForMimeList << app;
		}
	}

	return appsForMimeList;
};

QStringList NBAppEngine::mimeTypesForApp( QString desktopName ) {

	QStringList mimeList;

	if ( not desktopName.endsWith( ".desktop" ) )
		desktopName += ".desktop";

	foreach( QString appDir, appsDirs ) {
		if ( QFile::exists( appDir + desktopName ) ) {
			mimeList << NBAppFile( appDir + desktopName ).value( NBAppFile::MimeTypes ).toStringList();
			break;
		}
	}

	return mimeList;
};

NBAppsList NBAppEngine::allDesktops() {

	return appsList;
};

void NBAppEngine::parseDesktops() {

	QTime time = QTime::currentTime();
	time.start();
	foreach( QString appDir, appsDirs ) {
		foreach( QString application, listApplications( appDir ) ) {
			appsList << NBAppFile( appDir + application );
		}
	}
};

NBAppEngine* NBAppEngine::instance() {

	if ( NBAppEngine::globalInstance )
		return globalInstance;

	NBAppEngine::globalInstance = new NBAppEngine();
	globalInstance->parseDesktops();

	return NBAppEngine::globalInstance;
};

NBAppEngine::NBAppEngine() {

	appsDirs << QDir::home().filePath( ".local/share/applications/" );
	appsDirs << "/usr/local/share/applications/" << "/usr/share/applications/";
	appsDirs << "/usr/share/applications/kde4/" << "/usr/share/gnome/applications/";
};

QStringList NBAppEngine::listApplications( QString appsDir ) {

	QStringList applications;

	DIR *dir;
	struct dirent *ent;
	dir = opendir( qPrintable( appsDir ) );

	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {
			QString nodeName = QString::fromLocal8Bit( ent->d_name );
			if ( ( nodeName.compare( "." ) == 0 ) or ( nodeName.compare( ".." ) == 0 ) )
				continue;

			if ( nodeName.endsWith( ".desktop" ) )
				applications << nodeName;
		}

		closedir( dir );
	}

	return applications;
};

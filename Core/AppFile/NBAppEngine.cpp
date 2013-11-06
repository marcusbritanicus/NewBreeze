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
		QString name = app.value( NBAppFile::Name ).toString();
		foreach( QString mime, mimeList ) {
			if ( app.value( NBAppFile::MimeTypes ).toStringList().contains( mime ) ) {
				if ( ( app.value( NBAppFile::Type ).toString() == "Application" ) and ( not app.value( NBAppFile::NoDisplay ).toBool() ) ) {
					if ( not appsForMimeList.contains( app ) ) {
						appsForMimeList << app;
					}
				}
			}
		}
	}

	/*
		*
		* We have the appsForMimeList. Now we need to filter some applications out as per user's choice and get the default value
		* First check mimeapps.list/[Default Associations], then mimeapps.list/[Added Associations]. The entry corresponding to the mimetype in
		* the first case and the first entry in the second case are the user defaults.
		* If the mimetype is not listed, then check mimeinfo.cache
		*
		* Do the same for /usr/local/share/applications and /usr/share/applications
		*
	*/

	QString desktopName = NBAppEngine::xdgDefaultApp( mimeType.name() );
	for( int i = 0; i < appsForMimeList.count(); i++ ) {
		NBAppFile app = appsForMimeList.at( i );
		if ( app.desktopFileName() == desktopName ) {
			appsForMimeList.move( i, 0 );
			break;
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

void NBAppEngine::setApplicationAsDefault( QString appFileName, QString mimetype ) {

	if ( QProcess::execute( "xdg-mime", QStringList() << "default" << appFileName << mimetype ) )
		qDebug() << "Error while settings" << appFileName << "as the default handler for" << mimetype;
};

NBAppEngine::NBAppEngine() {

	appsDirs << QDir::home().filePath( ".local/share/applications/" );
	appsDirs << "/usr/local/share/applications/" << "/usr/share/applications/";
	appsDirs << "/usr/share/applications/kde4/" << "/usr/share/gnome/applications/";
};

QString NBAppEngine::xdgDefaultApp( QString mimeType ) {

	QStringList files;
	files << QDir::home().filePath( ".local/share/applications/mimeapps.list" );
	files << QDir::home().filePath( ".local/share/applications/defaults.list" );
	files << QDir::home().filePath( ".local/share/applications/mimeinfo.cache" );

	files << QString( "/usr/local/share/applications/mimeapps.list" );
	files << QString( "/usr/local/share/applications/defaults.list" );
	files << QString( "/usr/local/share/applications/mimeinfo.cache" );

	files << QString( "/usr/share/applications/mimeapps.list" );
	files << QString( "/usr/share/applications/defaults.list" );
	files << QString( "/usr/share/applications/mimeinfo.cache" );

	QString defaultValue;
	foreach( QString file, files ) {
		QSettings defaults( file, QSettings::NativeFormat );
		defaultValue = defaults.value( QString( "Default Applications/%1" ).arg( mimeType ) ).toString();
		if ( defaultValue.isEmpty() )
			defaultValue = defaults.value( QString( "Added Associations/%1" ).arg( mimeType ) ).toString();

		else
			break;

		if ( defaultValue.isEmpty() or defaultValue.isNull() )
			continue;

		else
			break;
	}

	return defaultValue;
};

NBAppFile NBAppEngine::xdgDefaultApp( QMimeType mimeType ) {

	return appsForMimeType( mimeType ).at( 0 );
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

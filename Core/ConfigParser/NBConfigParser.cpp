/*
	*
	* NBConfigParser.cpp - NewBreeze ConfigParser Class
	*
*/

// C++ Standard Library
#include <NBConfigParser.hpp>
#include <NBDesktopFile.hpp>

QStringList NBConfigParser::dirPaths;

NBConfigParser::NBConfigParser( QStringList dPths ) {

	dirPaths = dPths;
};

QMap<QString, QStringList> NBConfigParser::parseDir( QString dirPath ) {

	/*
		*
		*  Output is in the following format:
		* { "MimeType1" = [ "DesktopFileUrl1.1", "DesktopFileUrl1.2", "DesktopFileUrl1.3", ],
		*   "MimeType2" = [ "DesktopFileUrl2.1", "DesktopFileUrl2.2", "DesktopFileUrl2.3", ], }
		*
	*/

	QDir dir( dirPath );

	if ( !dir.exists() ) {
		// qDebug() << "Dir not found: " << dirPath;
		return QMap<QString, QStringList>();
	}

	QMap<QString, QStringList> mimeDesktopMap;

	dir.setFilter( QDir::Files );
	dir.setNameFilters( QStringList() << "*.desktop" );

	QStringList files = dir.entryList();
	QStringList desktopList;

	foreach( QString file, files ) {
		NBDesktopFile dFile( dir.absoluteFilePath( file ) );
		foreach( QString mimetype, dFile.mimetypes ) {
			QStringList dList = mimeDesktopMap.value( mimetype );
			dList << dFile.fileUrl;
			dList.removeDuplicates();
			mimeDesktopMap.insert( mimetype, dList );
		}
	}

	return mimeDesktopMap;
}

QMap<QString, QStringList> NBConfigParser::parseDirs( QStringList dPaths ) {

	QMap<QString, QStringList> parsed, mimeDesktopMap;

	foreach( QString dPath, dPaths ) {
		parsed = parseDir( dPath );
		foreach( QString mime, parsed.keys() ) {
			QStringList dList = mimeDesktopMap.value( mime );
			dList << parsed.value( mime );
			dList.removeDuplicates();
			mimeDesktopMap.insert( mime, dList );
		}
	}

	return mimeDesktopMap;
};

void NBConfigParser::createCache() {

	QTime time = QTime::currentTime();
	time.start();

	QSettings cache( mimeProgsCache, QSettings::NativeFormat );

	QMap<QString, QStringList> mimeDesktopMap = parseDirs( dirPaths );

	foreach( QString mimetype, mimeDesktopMap.keys() )
		cache.setValue( mimetype, mimeDesktopMap.value( mimetype ) );

	qDebug() << "Created the cache in" << time.elapsed() << "milliseconds";
};

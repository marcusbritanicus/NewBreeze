/*
	*
	* NBFileSystemWatcher.cpp - An advanced wrapper based on QFileSystemWatcher
	*
*/

#include <NBFileSystemWatcher.hpp>

QMutex mutex;

static inline QStringList entries( QString mPath ) {

	QStringList contents;

	DIR *dir;
	struct dirent *ent;
	dir = opendir( mPath.toLocal8Bit().constData() );

	if ( dir != NULL ) {
		while ( ( ent = readdir( dir ) ) != NULL) {

			/* Do not show . and .. */
			QString nodeName = QString::fromLocal8Bit( ent->d_name );
			if ( ( nodeName.compare( "." ) == 0 ) or ( nodeName.compare( ".." ) == 0 ) )
				continue;

			/* Show Hidden */
			if ( ent->d_type == DT_DIR )
				contents << mPath + nodeName + "/";

			else
				contents << mPath + nodeName;
		}

		closedir( dir );

		return contents;
	}

	return QStringList();
};

static inline QPair<QStringList, QStringList> difference( QStringList oldList, QStringList newList ) {

	QPair<QStringList, QStringList> pair;

	/* Items in first list not found in second: deleted */
	Q_FOREACH( QString itm, oldList ) {
		if ( not newList.contains( itm ) )
			pair.first << itm;
	}

	/* Items in second list not found in first: created */
	Q_FOREACH( QString itm, newList ) {
		if ( not oldList.contains( itm ) )
			pair.second << itm;
	}

	return pair;
};

NBFileSystemWatcher::NBFileSystemWatcher() : QFileSystemWatcher() {

	connect( this, SIGNAL( directoryChanged( QString ) ), this, SLOT( handleChanged( QString ) ) );
	connect( this, SIGNAL( fileChanged( QString ) ), this, SLOT( handleChanged( QString ) ) );

	renames.clear();
};

void NBFileSystemWatcher::setWatchPath( QString path ) {

	if ( not path.endsWith( "/" ) )
		path += "/";

	/* Remove old watches: directories */
	Q_FOREACH( QString path, directories() )
		removePath( path );

	/* Remove old watches: files */
	Q_FOREACH( QString path, files() )
		removePath( path );

	addPath( path );
	watchPath = QString( path );

	contents.clear();
	contents << entries( path );

	if ( QFileInfo( path ).isDir() ) {
		Q_FOREACH( QString entry, contents ) {
			addPath( entry );
		}
	}
};

bool NBFileSystemWatcher::isRunning() {

	return ( directories().count() and files().count() );
};

void NBFileSystemWatcher::stopWatch() {

	/* Remove old watches: directories */
	Q_FOREACH( QString path, directories() )
		removePath( path );

	/* Remove old watches: files */
	Q_FOREACH( QString path, files() )
		removePath( path );
};

void NBFileSystemWatcher::handleChanged( QString cPath ) {

	QMutexLocker locaker( &mutex );

	QFileInfo info( cPath );

	if ( info.isDir() and not cPath.endsWith( "/") )
		cPath += "/";

	if ( cPath != watchPath ) {
		/* True modification, not rename, delete, etc */
		if ( info.exists() )
			emit nodeChanged( cPath );

		/* False deletes due to renames */
		else if ( not renames.contains( cPath ) )
			emit nodeDeleted( cPath );

		return;
	}

	/* Fresh event. Clear renames */
	renames.clear();

	QStringList newContents = entries( watchPath );
	QPair<QStringList, QStringList> pair = difference( contents, newContents );

	/* Add the new nodes to watch */
	Q_FOREACH( QString node, pair.second )
		addPath( node );

	/* Remove the non existing nodes */
	Q_FOREACH( QString node, pair.first )
		removePath( node );

	/* First list empty, Second list has contents: Files added */
	/* Please not that the lists should technically have only one element each */
	/* The exception is as Qt points out changes in very quick succession */
	if ( not pair.first.size() and pair.second.size() ) {
		Q_FOREACH( QString node, pair.second )
			emit nodeCreated( node );
	}

	/*
		*
		* If the size of the lists is more than one, then we are screwed.
		* The reason is Qt: Multiple events in fast succession may cause only one signal
		* Hopefully this does not happen except in cases like batch renaming
		* Since the renames are one to one mapping, we can match randomly
		*
	*/
	else if ( pair.first.size() == pair.second.size() ) {
		for( int i = 0; i < pair.first.size(); i++ )
			emit nodeRenamed( pair.first.at( i ), pair.second.at( i ) );

		renames << pair.first;
	}

	/* First list has contents, second list empty: Files deleted */
	/* Please not that the lists should technically have only one element each */
	/* The exception is as Qt points out changes in very quick succession */
	else if ( pair.first.size() and not pair.second.size() ) {
		Q_FOREACH( QString node, pair.second )
			emit nodeDeleted( node );
	}

	/*
		*
		* This should not happen except in cases where several files have been deleted very fast and
		* several files have been added very fast resulting in just one signal. We just handle it to
		* avoid ugly consequences.
		*
	*/
	else if ( pair.first.size() and pair.second.size() ) {
		Q_FOREACH( QString node, pair.second )
			emit nodeDeleted( node );

		Q_FOREACH( QString node, pair.second )
			emit nodeCreated( node );
	}

	contents.clear();
	contents << newContents;
};

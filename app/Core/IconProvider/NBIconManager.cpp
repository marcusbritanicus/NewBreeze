/*
	*
	* NBIconManager.cpp - Icon Manager class for NewBreeze
	*
*/

/*
	*
	* The new Icon Manager replaces the old NBIconProvider. The functionalities will be enhanced greatly in this class.
	* New:
	* 		- Icons for images will be stored in a single massive database.
	* 		- Standard icon database will be created which matches a mimetype to icons paths for various sizes
	*				MimeType 1			Size 1				Path 1
	*									Size 2				Path 2
	*									Size 3				Path 3
	*		  This ensures that we do not have to search the same icon multiple times.
	* 		- We create this Standard Icon Database once using the startup wizard. Subsequent updation needs to be done manually.
	* 		- The database gets updated if the icon theme is changed.
	*
*/

#include "NBIconManager.hpp"

NBIconManager *NBIconManager::iMgr = NULL;

NBIconManager* NBIconManager::instance() {

	/* If the Plugin Manager has already been init, then return the instance */
	if ( iMgr and iMgr->init )
		return iMgr;

	/* Init our plugin manager */
	iMgr = new NBIconManager();

	/* If init succeeds, we set the init flag and return */
	iMgr->init = true;

	return iMgr;
};

NBIconManager::NBIconManager() {

	/* Set the default theme path */
	iconThemePath = QString( "/usr/share/icons/" );

	/* Initialize the Database */
	thumbsDb = QSqlDatabase::addDatabase( "QSQLITE" );
	thumbsDb.setDatabaseName( NBXdg::userDir( NBXdg::XDG_CACHE_HOME ) + "/NewBreeze/Thumbnails.db" );

	if ( not thumbsDb.open() ) {
		qCritical() << "Error opening thumbnail database. Dislaying thumbnails will be disabled. "
				"Pleas enable this manually again.";
		QSettings sett( "NewBreeze", "NewBreeze" );
		sett.setValue( "ImagePreviews", false );
		sett.sync();
	}

	/* Populate the hashes */
	readThemeDirectories();
	readThemeInheritance();
};

void NBIconManager::readThemeDirectories() {

	QDir iconThemeDir( iconThemePath );
	Q_FOREACH( QString theme, iconThemeDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot ) ) {
		QSettings index( iconThemePath + theme + "/index.theme", QSettings::NativeFormat );
		themeDirsHash[theme] = index.value( "Icon Theme/Directories" ).toStringList();
	}
};

void NBIconManager::readThemeInheritance() {

	QDir iconThemeDir( iconThemePath );
	Q_FOREACH( QString theme, iconThemeDir.entryList( QDir::AllDirs | QDir::NoDotAndDotDot ) ) {
		QSettings index( iconThemePath + theme + "/index.theme", QSettings::NativeFormat );
		themeFallbackHash[theme] = index.value( "Icon Theme/Inherits" ).toStringList();
	}
};

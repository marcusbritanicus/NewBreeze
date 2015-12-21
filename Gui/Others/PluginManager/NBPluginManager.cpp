/*
	*
	* NBPluginManager.cpp - Plugin Manager for NewBreeze
	*
*/

#include <NBPluginManager.hpp>

static inline QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( qPrintable( path ) );
	QString basePth = QString( basename( dupPath ) );
	free( dupPath );

	return basePth;
};

NBPluginManager *NBPluginManager::pMgr = NULL;

NBPluginManager* NBPluginManager::instance() {

	/* If the Plugin Manager has already been init, then return the instance */
	if ( pMgr and pMgr->init )
		return pMgr;

	/* Init our plugin manager */
	pMgr = new NBPluginManager();

	/* If init succeeds, we set the init flag and return */
	pMgr->init = true;

	return pMgr;
};

bool NBPluginManager::hasPluginForMimeType( QString mimeName ) {

	return mimePluginMap.value( mimeName ).count() ? true : false;
};

QString NBPluginManager::pluginForMimeType( QString mimeName ) {

	QStringList plugins = mimePluginMap.value( mimeName );

	if ( plugins.count() == 1 )
		return plugins.value( 0 );

	QString bestPlugin = plugins.value( 0 );
	Q_FOREACH( QString plugin, plugins ) {
		if ( pluginPriorityMap.value( baseName( plugin ) ) > pluginPriorityMap.value( baseName( bestPlugin ) ) )
			bestPlugin = plugin;
	}

	return bestPlugin;
};

NBPluginManager::NBPluginManager() {

	reloadPlugins();
};

void NBPluginManager::reloadPlugins() {

	/* For now we will be reading just the plugins folder of the home directory */
	QStringList pluginPaths = QStringList() << /*QString( DATA_DIR ) + "/plugins/" <<*/ NBXdg::home() + "/.config/NewBreeze/plugins/";

	/* Our MimeDatabase object */
	QMimeDatabase mdb;

	/* Clear the mimePluginMap and pluginPriorityMap */
	mimePluginMap.clear();
	pluginPriorityMap.clear();

	/* Prepare the map */
	Q_FOREACH( QMimeType mType, mdb.allMimeTypes() )
		mimePluginMap[ mType.name() ] = QStringList();

	/* For now we will be handling just the plugins based on the PreviewInterface */
	QDir pPath( NBXdg::home() + "/.config/NewBreeze/plugins/" );
	NBPreviewInterface *plugin = 0;
	QObject *pObj = 0;
	Q_FOREACH( QString pluginSo, pPath.entryList( QStringList() << "*.so", QDir::Files, QDir::Name | QDir::IgnoreCase ) ) {
		QPluginLoader loader( pPath.filePath( pluginSo ) );
		pObj = loader.instance();
		if ( pObj ) {
			plugin = qobject_cast<NBPreviewInterface*>( pObj );
			Q_FOREACH( QString mime, plugin->mimeTypesHandled() )
				mimePluginMap[ mime ] << pPath.filePath( pluginSo );
		}
		loader.unload();
	}

	QSettings pluginPriorities( "NewBreeze", "Plugins" );
	pluginPriorities.beginGroup( "Priorities" );
	Q_FOREACH( QString plugin, pluginPriorities.childKeys() )
		pluginPriorityMap[ plugin ] = pluginPriorities.value( plugin ).toInt();
};

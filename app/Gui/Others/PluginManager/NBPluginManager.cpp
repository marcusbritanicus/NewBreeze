/*
	*
	* NBPluginManager.cpp - Plugin Manager for NewBreeze
	*
*/

#include "NBPluginManager.hpp"

QList<NBPluginInterface::Interface> developedInterfaces;

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

	if ( getuid() )
		return mimePluginMap.value( mimeName ).count() ? true : false;

	else
		return false;
};

QString NBPluginManager::pluginForMimeType( QString mimeName ) {

	if ( not getuid() )
		return QString();

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

PluginList NBPluginManager::plugins( NBPluginInterface::Interface iface, NBPluginInterface::Type type, NBPluginInterface::Context ctxt, QString mime ) {

	PluginList plugList;
	if ( not getuid() )
		return plugList;

	Q_UNUSED( type );

	NBContextPluginHash cph = mPluginsHash.value( iface );
	Q_FOREACH( NBPluginInterface *plugin, cph.value( ctxt ) ) {
		if ( plugin->mimetypes().contains( mime ) or plugin->mimetypes().contains( "*" ) )
			plugList << plugin;
	}

	return plugList;
};

PluginList NBPluginManager::allPlugins() {

	return mPluginList;
};

NBPluginManager::NBPluginManager() {

	reloadPlugins();
};

void NBPluginManager::reloadPlugins() {

	if ( not getuid() )
		return;

	reloadPeekPlugins();
	reloadOtherPlugins();
};

void NBPluginManager::reloadPeekPlugins() {
};

void NBPluginManager::reloadOtherPlugins() {

	QSettings nbsett( "NewBreeze", "Plugins" );

	QStringList pluginPaths;
	pluginPaths << nbsett.value( "PluginPaths", QStringList() << "./plugins" ).toStringList();

	int parsed = 0;
	int added = 0;

	mPluginsHash.clear();
	Q_FOREACH( QString path, pluginPaths ) {
		QDir pPathDir( path );
		Q_FOREACH( QString pluginSo, pPathDir.entryList( QStringList() << "*.so", QDir::Files, QDir::Name ) ) {
			parsed++;
			QPluginLoader loader( pPathDir.absoluteFilePath( pluginSo ) );
			QObject *pObject = loader.instance();
			if ( pObject ) {
				NBPluginInterface *plugin = qobject_cast<NBPluginInterface*>( pObject );
				if ( not plugin )
					continue;

				mPluginList << plugin;
				added++;
				Q_FOREACH( NBPluginInterface::Interface iface, plugin->interfaces() ) {
					NBContextPluginHash cph = mPluginsHash.value( iface );
					Q_FOREACH( NBPluginInterface::Context ctxt, plugin->contexts( iface ) ) {
						PluginList pl = cph.value( ctxt );
						if ( not pl.contains( plugin ) )
							pl << plugin;

						cph[ ctxt ] = pl;
					}

					mPluginsHash[ iface ] = cph;
				}
			}
		}
	}
};

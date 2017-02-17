/*
	*
	* NBPluginManager.cpp - Plugin Manager for NewBreeze
	*
*/

#include "NBPluginManager.hpp"

static QList<NBPluginInterface::Interface> developedInterfaces;

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

	/* For now we will be reading just the plugins folder of the home directory */
	QStringList pluginPaths;
	#if QT_VERSION >= 0x050000
		pluginPaths << "/usr/lib/newbreeze/plugins5/" << NBXdg::home() + "/.config/NewBreeze/plugins5/";
	#else
		pluginPaths << "/usr/lib/newbreeze/plugins/" << NBXdg::home() + "/.config/NewBreeze/plugins/";
	#endif

	/* Our MimeDatabase object */
	QMimeDatabase mdb;

	/* Clear the mimePluginMap and pluginPriorityMap */
	mimePluginMap.clear();
	pluginPriorityMap.clear();

	/* Prepare the map */
	Q_FOREACH( QMimeType mType, mdb.allMimeTypes() )
		mimePluginMap[ mType.name() ] = QStringList();

	/* For now we will be handling just the plugins based on the PreviewInterface */
	QDir pPathDir;
	#if QT_VERSION >= 0x050000
		pPathDir.setPath( NBXdg::home() + "/.config/NewBreeze/plugins5/" );
	#else
		pPathDir.setPath( NBXdg::home() + "/.config/NewBreeze/plugins/" );
	#endif

	Q_FOREACH( QString pluginSo, pPathDir.entryList( QDir::Files ) ) {
		QPluginLoader loader( pPathDir.absoluteFilePath( pluginSo ) );
		QObject *plugin = loader.instance();
		if ( plugin ) {
			NBPreviewInterface *interface = qobject_cast<NBPreviewInterface*>( plugin );
			if ( not interface )
				continue;

			Q_FOREACH( QString mime, interface->mimeTypesHandled() )
				mimePluginMap[ mime ] << pPathDir.absoluteFilePath( pluginSo );
		}
	}

	QSettings pluginPriorities( "NewBreeze", "Plugins" );
	pluginPriorities.beginGroup( "Priorities" );
	Q_FOREACH( QString plugin, pluginPriorities.childKeys() )
		pluginPriorityMap[ plugin ] = pluginPriorities.value( plugin ).toInt();

	pluginPriorities.endGroup();
	pluginPriorities.sync();
};

void NBPluginManager::reloadOtherPlugins() {

	QStringList pluginPaths;
	#if QT_VERSION >= 0x050000
		pluginPaths << "/usr/lib/newbreeze/plugins5/" << NBXdg::home() + "/.config/NewBreeze/plugins5/";
	#else
		pluginPaths << "/usr/lib/newbreeze/plugins/" << NBXdg::home() + "/.config/NewBreeze/plugins/";
	#endif

	mPluginsHash.clear();
	Q_FOREACH( QString path, pluginPaths ) {
		QDir pPathDir( path );
		Q_FOREACH( QString pluginSo, pPathDir.entryList( QStringList() << "*.so", QDir::Files, QDir::Name ) ) {
			QPluginLoader loader( pPathDir.absoluteFilePath( pluginSo ) );
			QObject *pObject = loader.instance();
			if ( pObject ) {
				NBPluginInterface *plugin = qobject_cast<NBPluginInterface*>( pObject );
				if ( not plugin )
					continue;

				mPluginList << plugin;
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

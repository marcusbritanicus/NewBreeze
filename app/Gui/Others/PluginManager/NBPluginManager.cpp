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

PluginList NBPluginManager::plugins( NBPluginInterface::Interface iface, NBPluginInterface::Type type, NBPluginInterface::Context ctxt, QString mime ) {

	PluginList plugins;
	Q_FOREACH( PluginCapability *pCap, mPluginCapabilityList ) {
		bool truth = true;
		truth &= ( pCap->interface == iface );
		truth &= ( pCap->type == type );
		truth &= (bool)pCap->contexts.contains( ctxt );
		if ( mime.count() )
			truth &= (bool)pCap->mimeTypes.contains( mime );

		if ( truth )
			plugins << pCap->plugin;
	}

	return plugins;
};

NBPluginManager::NBPluginManager() {

	reloadPlugins();
};

void NBPluginManager::reloadPlugins() {

	reloadPeekPlugins();
	reloadOtherPlugins();
};

void NBPluginManager::reloadPeekPlugins() {

	/* For now we will be reading just the plugins folder of the home directory */
	QStringList pluginPaths;
	#if QT_VERSION >= 0x050000
		pluginPaths << "/usr/share/newbreeze/plugins5/" << NBXdg::home() + "/.config/NewBreeze/plugins5/";
	#else
		pluginPaths << "/usr/share/newbreeze/plugins/" << NBXdg::home() + "/.config/NewBreeze/plugins/";
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

	/* For now we will be reading just the plugins folder of the home directory */
	QStringList pluginPaths;
	#if QT_VERSION >= 0x050000
		pluginPaths << "/usr/share/newbreeze/plugins5/" << NBXdg::home() + "/.config/NewBreeze/plugins5/";
	#else
		pluginPaths << "/usr/share/newbreeze/plugins/" << NBXdg::home() + "/.config/NewBreeze/plugins/";
	#endif

	QStringList pluginNames;
	Q_FOREACH( QString path, pluginPaths ) {
		QDir pPathDir( path );
		Q_FOREACH( QString pluginSo, pPathDir.entryList( QStringList() << "*.so", QDir::Files, QDir::Name ) ) {
			QPluginLoader loader( pPathDir.absoluteFilePath( pluginSo ) );
			QObject *plugin = loader.instance();
			if ( plugin ) {
				NBPluginInterface *interface = qobject_cast<NBPluginInterface*>( plugin );
				if ( not interface )
					continue;

				if ( pluginNames.contains( pluginSo ) ) {
					PluginCapability *oldPCap = mPluginCapabilityList.at( pluginNames.indexOf( pluginSo ) );
					/* If there is an newer version already registered, we skip this one */
					if ( oldPCap->version > interface->version() ) {
						continue;
					}

					/* We remove the existing instance and insert a new one */
					else {
						mPluginCapabilityList.removeAt( pluginNames.indexOf( pluginSo ) );
						pluginNames.removeAll( pluginSo );
					}
				}

				pluginNames << pluginSo;
				PluginCapability *pcap = new PluginCapability;
				pcap->name = pPathDir.absoluteFilePath( pluginSo );
				pcap->interface = interface->interface();
				pcap->type = interface->type();
				pcap->contexts = interface->contexts();
				pcap->mimeTypes = interface->mimetypes();
				pcap->plugin = interface;

				mPluginCapabilityList << pcap;
			}
		}
	}
};

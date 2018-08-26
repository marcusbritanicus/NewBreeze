/*
	*
	* NBPluginManager.hpp - NBPluginManager.cpp Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBPluginInterface.hpp"

/* PluginList */
typedef QList<NBPluginInterface*> PluginList;

/* PluginList of Context <int> */
typedef QHash<int, PluginList> NBContextPluginHash;

class NBPluginManager : public QObject {
	Q_OBJECT

	public:
		/* Create a global instance of NBPluginManager */
		static NBPluginManager* instance();

		/* Return the plugin path for the given mimetype */
		bool hasPluginForMimeType( QString );

		/* Return the plugin path for the given mimetype */
		QString pluginForMimeType( QString );

		/* Return the plugin path for the given setup */
		PluginList plugins( NBPluginInterface::Interface, NBPluginInterface::Type, NBPluginInterface::Context, QString );

		/* Return all the unique plugins */
		PluginList allPlugins();

	public Q_SLOTS:
		/* Reload the plugins map */
		void reloadPlugins();

	private:
		NBPluginManager();

		/* Global Instance */
		static NBPluginManager* pMgr;

		/* Init flag */
		bool init;

		/* PluginList */
		PluginList mPluginList;

		/* MimeType-PluginPath map */
		QMap<QString, QStringList> mimePluginMap;
		QMap<QString, int> pluginPriorityMap;

		/* Interface-Context-PluginList Hash */
		QHash<int, NBContextPluginHash> mPluginsHash;
};

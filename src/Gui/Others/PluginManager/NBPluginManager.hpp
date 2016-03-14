/*
	*
	* NBPluginManager.hpp - NBPluginManager.cpp Header
	*
*/

#pragma once

#include <QtCore>
#include <NBPluginInterface.hpp>
#include <NBPreviewInterface.hpp>
#include <NBMimeDatabase.hpp>
#include <NBXdg.hpp>

class NBPluginManager {

	public:
		/* Create a global instance of NBPluginManager */
		static NBPluginManager* instance();

		/* Return the plugin path for the given mimetype */
		bool hasPluginForMimeType( QString );

		/* Return the plugin path for the given mimetype */
		QString pluginForMimeType( QString );

	public Q_SLOTS:
		/* Reload the plugins map */
		void reloadPlugins();

	private:
		NBPluginManager();

		/* Global Instance */
		static NBPluginManager* pMgr;

		/* Init flag */
		bool init;

		/* MimeType-PluginPath map */
		QMap<QString, QStringList> mimePluginMap;
		QMap<QString, int> pluginPriorityMap;
};

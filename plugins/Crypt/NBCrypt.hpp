/*
	*
	* NBCrypt.hpp - NewBreeze File Folder Encryption Class Header
	*
*/

#pragma once

#include "Global.hpp"
#include "NBEncFS.hpp"
#include "NBSalsa20.hpp"
#include "NBTools.hpp"

#include <QtPlugin>
#include "NBPluginInterface.hpp"

class NBCrypt : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "Crypt5.json" )
	#endif
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		QString name();

		/* The plugin version */
		QString version();

		/* The QAction hooks for menus/toolbars */
		QList<QAction*> actions( Interface, QStringList );

		/* Interface type: preview, rename etc */
		NBPluginInterface::Interfaces interfaces();

		/* Interface type: preview, rename etc */
		NBPluginInterface::Type type( Interface iface );

		/* Plugin load contexts */
		NBPluginInterface::Contexts contexts( Interface iface );

		/* Mimetypes handled by the plugin */
		QStringList mimetypes();

		/* Invoke slots called called by triggering the actions */
		void actionTrigger( Interface, QString, QStringList nodes );

		/* Store the called widget pointer */
		void setCaller( QWidget* );
};

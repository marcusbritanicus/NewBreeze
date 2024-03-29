/*
	*
	* NBThumbnailerPlugin.hpp - NewBreeze Video Thumbnailer Class
	*
*/

#pragma once

#include "Global.hpp"
#include <newbreeze/NBPluginInterface.hpp>

#include <QtPlugin>

class NBThumbnailerPlugin : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "Thumbnailer.json" )
	#endif
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		 QString name();

		/* The plugin version */
		 QString version();

		/* The QAction */
		 QList<QAction*> actions( Interface, QStringList );

		/* Interface type: preview, rename etc */
		 Interfaces interfaces();

		/* Interface type: preview, rename etc */
		 Type type( Interface );

		/* Plugin load context */
		 Contexts contexts( Interface );

		/* Mimetypes handled by the plugin */
		 QStringList mimetypes();

		/* Invoke slots called called by triggering the actions */
		void actionTrigger( Interface, QString, QStringList );

		/* Store the called widget pointer */
		void setCaller( QWidget *caller );

	private:
		void makeVideoThumbnail( QString, QString );
		void makeDjVuThumbnail( QString, QString );
};

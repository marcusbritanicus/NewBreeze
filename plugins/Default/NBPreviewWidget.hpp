/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include "NBPluginInterface.hpp"

#include "NBDjvuDisplay.hpp"
#include "NBImagePeek.hpp"
#include "NBOdfOgle.hpp"
#include "NBPdfPeep.hpp"
#include "NBWebWatch.hpp"
#include "NBWordView.hpp"

#include <QtPlugin>
#include "Global.hpp"

class NBPreviewWidget : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "DefaultPeekPlugins5.json" )
	#endif
	Q_INTERFACES( NBPluginInterface )

	public:
		/* Name of the plugin */
		 QString name();

		/* The plugin version */
		 QString version();

		/* The QAction */
		 QList<QAction*> actions( QStringList );

		/* Interface type: preview, rename etc */
		 Interface interface();

		/* Interface type: preview, rename etc */
		 Type type();

		/* Plugin load context */
		 Contexts contexts();

		/* Mimetypes handled by the plugin */
		 QStringList mimetypes();

		/* Store the called widget pointer */
		 void setCaller( QWidget *caller );
};

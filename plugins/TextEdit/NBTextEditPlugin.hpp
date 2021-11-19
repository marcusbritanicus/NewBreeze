/*
	*
	* TextEditor.hpp - NewBreeze File Folder Encryption Class Header
	*
*/

#pragma once

#include <newbreeze/common.hpp>
#include "TextEditor.hpp"

#include <QtPlugin>
#include <newbreeze/NBPluginInterface.hpp>

class TextEditorPlugin : public QObject, NBPluginInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPluginInterface" FILE "TextEdit5.json" )
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
		NBPluginInterface::Type type( Interface );

		/* Plugin load contexts */
		NBPluginInterface::Contexts contexts( Interface );

		/* Mimetypes handled by the plugin */
		QStringList mimetypes();

		/* Invoke slots called called by triggering the actions */
		void actionTrigger( Interface, QString, QStringList );

		/* Store the called widget pointer */
		void setCaller( QWidget* );

	private:
		TextEditor *Editor;
};

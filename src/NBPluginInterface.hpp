/*
	*
	* NBPluginInterface.hpp - NewBreeze PluginInterface Class
	*
	* This the general plugin interface. Unlike the PreviewInterface, this plugin
	* is meant to enhance the working of NewBreeze widgets directly, or provide a
	* replacement for some widgets. Please note that replacements cannot be
	* provided for all widgets, NBFolderView, NBAddressBar, NBProcessManager to name
	* a few.
	*
	* Note:
	* This PluginInterface can be used to enhance the NBFolderView class by providing
	* a suitable context. For the context to be applicable, the plugin must return
	* on of the FolderView interfaces for interface() and 'Enhancement' for type().
	* Otherwise 'context()' will be ignored and the plugin may not work.
	*
	* Note:
	* At the present moment, I have not enabled plugin interfaces for all the widgets
	* and hence your plugins may not work. If you have developed a plugin for some
	* interface, and if you know how to develop the interface for the widget, please
	* feel free to contribute them both to NewBreeze via github.
	*
*/

#pragma once

#include <QString>
#include <QStringList>
#include <QWidget>

class NBPluginInterface {

	public:
		/* Plugin Interface: Where will the plugin be used */
		enum Interface {
			AnyInterface			= 0xC5F4A6,			// Show for all interfaces
			SideBarInterface,							// Alternative/Enhancement for sidebar
			InfoBarInterface,							// Alternative for infobar
			RenameInterface,							// Alternative/Enhancement for Rename
			MimeTypeInterface,							// Special handling ofspecific mimetypes
			CreateInterface,							// Creation of new types of nodes
		};

		/* Plugin Type: How does the plugin work */
		enum Type {
			Alternative				= 0x65F8E4,			// Alternative for the existing widget. Ex: Mac sidebar instead of NBSidePanel
			Enhancement,								// Enhancement for the existing widget. Ex: Encrypted Folder list for NBSidePanel
		};

		/* Plugin context: When will the plugin be activated */
		enum Context {
			Any						= 0xD50BE8,			// Any Selection
			File,										// Single File Selection
			Dir,										// Single Folder Selection
			Node,										// Single File/Folder Selection
			Files,										// Multiple File Selection
			Dirs,    			 						// Multiple Folder Selection
			None										// This is applicable for selections
		};

		virtual ~NBPluginInterface() {};

		/* Name of the plugin */
		virtual QString name() = 0;

		/* The plugin version */
		virtual QString version() = 0;

		/* The QAction hooks for menus/toolbars */
		virtual QList<QAction*> actions() = 0;

		/* Plugin load context */
		virtual Context context() = 0;

		/* Interface type: preview, rename etc */
		virtual Interface interface() = 0;

		/* Interface type: preview, rename etc */
		virtual Type type() = 0;

		/* Mimetypes handled by the plugin */
		virtual QString mimetypes() = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPluginInterface, "com.NewBreeze.NBPluginInterface")
QT_END_NAMESPACE

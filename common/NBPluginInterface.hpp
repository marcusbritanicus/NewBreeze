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
	* Note1:
	* This PluginInterface can be used to enhance the NBFolderView class by providing
	* a suitable context. For the context to be applicable, the plugin must return
	* one of the FolderView interfaces for interface() and 'Enhancement' for type().
	* Otherwise 'context()' will be ignored and the plugin may not work.
	*
	* Note2:
	* At the present moment, I have not enabled plugin interfaces for all the widgets
	* and hence your plugins may not work. If you have developed a plugin for some
	* interface, and if you know how to develop the interface for the widget, please
	* feel free to contribute them both to NewBreeze via github.
	*
	* Note3:
	* actions(...) and actionsTrigger(...) are meant to be equivalent functions. If
	* provided the same conditions, they behave similarly. Instead of triggering an
	* action named 'act', for an interface named 'iface' for listed 'nodes', calling
	* actionsTrigger( iface, act, nodes ) gives the same result.
	*
*/

#pragma once

#include <QString>
#include <QStringList>
#include <QWidget>

#if defined NBPLUGIN
	#define PLUGIN_DLLSPEC Q_DECL_EXPORT
#else
	#define PLUGIN_DLLSPEC Q_DECL_IMPORT
#endif

class PLUGIN_DLLSPEC NBPluginInterface {

	public:
		/* Plugin Interface: Where will the plugin be used */
		enum Interface {
			AnyInterface			= 0xC5F4A6,			// Show for all interfaces
			PreviewInterface,							// File Preview Enhancements											> Peek (Ctrl+Return)
			InfoBarInterface,							// Alternative for infobar												> No actions
			TerminalInterface,							// Specialized Terminal plugin											> Open terminal (F4)
			RenameInterface,							// Alternative/Enhancement for Rename									> Rename (F2)
			MimeTypeInterface,							// Special handling of specific mimetypes								> 'Actions' Menu
			CreateInterface,							// Creation of new types of nodes										> 'New' Menu
			SendToInterface,							// Enhancement for Send to menu											> 'Send to' Menu
			ActionInterface,							// Enhancement for ContextMenu											> 'Actions' Menu
			PropertiesInterface,						// Alternative for Properties Dialog									> Properties (Alt+Return)
			PermissionsInterface,						// Alternative for ContextMenu											> Permissions (Alt+Shift+Return)
		};

		/* Plugin Type: How does the plugin work */
		enum Type {
			Alternative				= 0x65F8E4,			// Alternative for the existing widget. Ex: Classic sidebar instead of NBSidePanel
			Enhancement,								// Enhancement for the existing widget. Ex: EncFS Support for NewBreeze
		};

		/* Plugin context: The plugin will be activated when */
		enum Context {
			File					= 0xD50BE8,			// a SINGLE file is selected
			Dir,										// a SINGLE folder is selected
			Node,										// a SINGLE node is selected ( File, Dir, FIFO, Socket, Chr, Block, etc... )
			Files,										// MULTIPLE file are selected
			Dirs,    			 						// MULTIPLE folder are selected
			Nodes,										// MULTIPLE nodes are selected ( File, Dir, FIFO, Socket, Chr, Block, etc... )
			None										// there is no selection
		};

		typedef QList<Interface> Interfaces;
		typedef QList<Context> Contexts;

		virtual ~NBPluginInterface() {};

		/* Name of the plugin */
		virtual QString name() = 0;

		/* The plugin version */
		virtual QString version() = 0;

		/* The QAction */
		virtual QList<QAction*> actions( Interface, QStringList ) = 0;

		/* Interface: preview, rename etc */
		virtual Interfaces interfaces() = 0;

		/* Interface type: preview, rename etc */
		virtual Type type( Interface ) = 0;

		/* Plugin load context */
		virtual Contexts contexts( Interface ) = 0;

		/* Mimetypes handled by the plugin */
		virtual QStringList mimetypes() = 0;

		/* Invoke slots called called by triggering the actions */
		virtual void actionTrigger( Interface, QString, QStringList ) = 0;

		/* Store the called widget pointer */
		virtual void setCaller( QWidget *caller ) = 0;

	protected:
		QWidget *mParent = NULL;
};

QT_BEGIN_NAMESPACE
	Q_DECLARE_INTERFACE( NBPluginInterface, "com.NewBreeze.NBPluginInterface")
QT_END_NAMESPACE

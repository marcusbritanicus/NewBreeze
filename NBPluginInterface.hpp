/*
	*
	* NBPluginInterface.hpp - NewBreeze PluginInterface Class
	*
*/

#pragma once

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QWidget;
QT_END_NAMESPACE

class NBPluginInterface {

	public:
		enum Context {
			Any       = 0x00,			// Any Selection
			File      = 0x01,			// Single File Selection
			Dir       = 0x02,			// Single Folder Selection
			Node      = 0x03,			// Single File/Folder Selection
			Files     = 0x04,			// Multiple File Selection
			Dirs      = 0x05			// Multiple Folder Selection
		};

		virtual ~NBPluginInterface() {};

		/* Interface type: preview, rename etc */
		virtual QString interface() = 0;

		/* Name of the plugin */
		virtual QString name() = 0;

		/* The QAction hook for menus/toolbars */
		virtual QAction action() = 0;

		/* The plugin version */
		virtual QString version() = 0;

		/* Plugin load context */
		virtual Context context() = 0;

		/* Mimetypes handled by the plugin */
		virtual QString mimetypes() = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPluginInterface, "com.trolltech.NewBreeze.NBPluginInterface/1.0")
QT_END_NAMESPACE

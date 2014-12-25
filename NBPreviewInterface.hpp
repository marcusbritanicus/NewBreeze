/*
	*
	* NBPluginInterface.hpp - NewBreeze PluginInterface Class
	*
*/

#pragma once
#ifndef NBPLUGININTERFACE_HPP
#define NBPLUGININTERFACE_HPP

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QWidget;
QT_END_NAMESPACE

class NBPeekPluginInterface {

	public:
		virtual ~NBPeekPluginInterface() {};
		virtual QStringList mimeTypesHandled() const = 0;
		virtual QWidget* getPreviewWidget( const QString & ) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPluginInterface, "com.trolltech.NewBreeze.NBPeekPluginInterface/1.0")
QT_END_NAMESPACE

#endif

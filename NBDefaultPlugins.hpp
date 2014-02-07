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

class NBPeekPlugin {

	public:
		virtual ~NBPeekPlugin() {};
		virtual QWidget* imagePreviewWidget( const QString & ) = 0;
		virtual QWidget* odfPreviewWidget( const QString & ) = 0;
		virtual QWidget* pdfPreviewWidget( const QString & ) = 0;
		virtual QWidget* htmlPreviewWidget( const QString & ) = 0;
		virtual QWidget* textPreviewWidget( const QString & ) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPeekPlugin, "com.trolltech.NewBreeze.NBPeekPlugin/1.0")
QT_END_NAMESPACE

#endif

/*
	*
	* NBPluginInterface.hpp - NewBreeze PluginInterface Class
	*
*/

#pragma once
#ifndef NBPREVIEWINTERFACE_HPP
#define NBPREVIEWINTERFACE_HPP

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QDialog;
QT_END_NAMESPACE

class NBPreviewInterface {

	public:
		virtual ~NBPreviewInterface() {};

		/* Return the list of mimetypes handled by this plugin */
		virtual QStringList mimeTypesHandled() const = 0;

		/* Setup and return the previewWidget that shows the file given as the argument */
		virtual QDialog* getPreviewWidget( const QString & ) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPreviewInterface, "com.trolltech.NewBreeze.NBPreviewInterface/1.0")
QT_END_NAMESPACE

#endif

/*
	*
	* NBPluginInterface.hpp - NewBreeze PluginInterface Class
	*
*/

#pragma once

#include <QString>
#include <QStringList>
#include <QDialog>

class NBPreviewInterface {

	public:
		virtual ~NBPreviewInterface() {};

		/* Return the list of mimetypes handled by this plugin */
		virtual QStringList mimeTypesHandled() const = 0;

		/* Setup and return the previewWidget that shows the file given as the argument */
		virtual QDialog* getPreviewWidget( const QString & ) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE( NBPreviewInterface, "org.NewBreeze.NBPreviewInterface" )
QT_END_NAMESPACE

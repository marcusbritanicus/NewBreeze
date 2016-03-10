/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#pragma once

#include <NBPreviewInterface.hpp>

#include <NBDjvuDisplay.hpp>
#include <NBImagePeek.hpp>
#include <NBOdfOgle.hpp>
#include <NBPdfPeep.hpp>
#include <NBWebWatch.hpp>
#include <NBWordView.hpp>

#include <QtPlugin>
#include <Global.hpp>

#if QT_VERSION < 0x050000
	#define Q_DECL_OVERRIDE
#endif

class NBPreviewWidget : public QObject, NBPreviewInterface {
	Q_OBJECT
	#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA( IID "org.NewBreeze.NBPreviewInterface" FILE "DefaultPeekPlugins5.json" )
	#endif
	Q_INTERFACES( NBPreviewInterface )

	public:
		QStringList mimeTypesHandled() const Q_DECL_OVERRIDE;
		QDialog* getPreviewWidget( const QString & ) Q_DECL_OVERRIDE;
};

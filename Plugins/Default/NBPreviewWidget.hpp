/*
	*
	* NBPreviewWidget.hpp - NewBreeze PreviewWidget Class Header
	*
*/

#ifndef NBPREVIEWWIDGET_HPP
#define NBPREVIEWWIDGET_HPP

#include <NBPreviewInterface.hpp>
#include <NBDjvuDisplay.hpp>
#include <NBImagePeek.hpp>
#include <NBOdfOgle.hpp>
#include <NBPdfPeep.hpp>
#include <NBWebWatch.hpp>
#include <NBWordView.hpp>

QT_BEGIN_NAMESPACE
class QString;
class QStringList;
class QWidget;
QT_END_NAMESPACE

class NBPreviewWidget : public QObject, public NBPreviewInterface {

	Q_OBJECT
	Q_INTERFACES( NBPreviewInterface )

	public:
		QStringList mimeTypesHandled() const;
		QDialog* getPreviewWidget( const QString & );
};

#endif

/*
	*
	* PopplerDocument - PopplerDocument class header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
	#include <QtWidgets>
#else
	#include <poppler-qt4.h>
#endif

#include "PdfDocument.hpp"

typedef Poppler::Page PdfPage;
typedef QList<Poppler::Page*> PdfPages;

class PopplerDocument : public PdfDocument {
	Q_OBJECT

	public:
		PopplerDocument( QString pdfPath );

		/* Check if a password is needed */
		bool passwordNeeded() const;

		/* Set a password */
		void setPassword( QString password );

		/* Pdf File Name and File Path */
		QString pdfName() const;
		QString pdfPath() const;

		/* Number of pages */
		int pageCount() const;

		/* Size of the page */
		QSizeF pageSize( int pageNo ) const;

		/* Reload the current document */
		void reload();

		/* Render and return a page */
		QImage renderPage( int ) const;
		QImage renderPageForWidth( int, qreal ) const;
		QImage renderPageForHeight( int, qreal ) const;

		/* Page Text */
		QString pageText( int pageNo ) const;

		/* Text of a Selection rectangle */
		QString text( int pageNo, QRectF ) const;

		qreal zoomForWidth( int pageNo, qreal width ) const;
		qreal zoomForHeight( int pageNo, qreal width ) const;

		void setZoom( qreal zoom ) {

			mZoom = zoom;
		};

		bool isReady() const {

			return mLoaded;
		}

	public Q_SLOTS:
		void loadDocument();

	private:
		Poppler::Document *mPdfDoc;

		QString mPdfPath;
		PdfPages mPages;

		qreal mZoom;

		bool mLoaded;
		bool mPassNeeded;
};

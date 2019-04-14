/*
	*
	* AbstractPdfDocument - AbstractPdfDocument class header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

class PdfDocument : public QObject {
	Q_OBJECT

	public:
		PdfDocument( QString ) {
		};

		/* Check if a password is needed */
		virtual bool passwordNeeded() const = 0;

		/* Set a password */
		virtual void setPassword( QString ) = 0;

		/* Pdf File Name and File Path */
		virtual QString pdfName() const = 0;

		virtual QString pdfPath() const = 0;

		/* Number of pages */
		virtual int pageCount() const = 0;

		/* Size of the page */
		virtual QSizeF pageSize( int ) const = 0;

		/* Reload the current document */
		virtual void reload() = 0;

		/* Render and return a page */
		virtual QImage renderPage( int ) const = 0;

		/* Render and return a page */
		virtual QImage renderPageForWidth( int, qreal ) const = 0;

		/* Render and return a page */
		virtual QImage renderPageForHeight( int, qreal ) const = 0;

		/* Page Text */
		virtual QString pageText( int ) const = 0;

		/* Text of a Selection rectangle */
		virtual QString text( int, QRectF ) const = 0;

		virtual qreal zoomForWidth( int, qreal ) const = 0;

		virtual qreal zoomForHeight( int, qreal ) const = 0;

		/* Pdf scale factor */
		virtual void setZoom( qreal ) = 0;

		virtual bool isReady() const = 0;

	public Q_SLOTS:
		virtual void loadDocument() = 0;

	Q_SIGNALS:
		void pdfLoaded();
		void loadFailed();
};

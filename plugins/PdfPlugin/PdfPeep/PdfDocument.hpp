/*
	*
	* AbstractPdfDocument - AbstractPdfDocument class header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "MuPdfDocument.hpp"
#include "PopplerDocument.hpp"

class PdfDocument : public QObject {
	Q_OBJECT

	public:
		enum {
			MuPdfRenderBackend = 0xC32C65,
			PopplerRenderBackend
		};

		PdfDocument( QString path, int rndrBknd ) {

			mRenderBackend = rndrBknd;

			if ( mRenderBackend == MuPdfRenderBackend ) {
				muDoc =  new MuPdfDocument( path );
				muDoc->loadDocument();

				connect( muDoc, SIGNAL( loadFailed() ), this, SIGNAL( loadFailed() ) );
				connect( muDoc, SIGNAL( pdfLoaded() ), this, SIGNAL( pdfLoaded() ) );

				popplerDoc = 0;
			}

			else {
				muDoc = 0;

				popplerDoc = new PopplerDocument( path );
				connect( popplerDoc, SIGNAL( loadFailed() ), this, SIGNAL( loadFailed() ) );
				connect( popplerDoc, SIGNAL( pdfLoaded() ), this, SIGNAL( pdfLoaded() ) );
			}
		};

		/* Check if a password is needed */
		bool passwordNeeded() const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->passwordNeeded();
			}

			else {

				return popplerDoc->passwordNeeded();
			}

			return false;
		};

		/* Set a password */
		void setPassword( QString pass ) {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->setPassword( pass );
			}

			else {

				return popplerDoc->setPassword( pass );
			}
		};

		/* Pdf File Name and File Path */
		QString pdfName() const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->pdfName();
			}

			else {

				return popplerDoc->pdfName();
			}

			return QString();
		};

		QString pdfPath() const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->pdfPath();
			}

			else {

				return popplerDoc->pdfPath();
			}

			return QString();
		};

		/* Number of pages */
		int pageCount() const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->pageCount();
			}

			else {

				return popplerDoc->pageCount();
			}

			return 0;
		};

		/* Size of the page */
		QSizeF pageSize( int pgNo ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->pageSize( pgNo );
			}

			else {

				return popplerDoc->pageSize( pgNo );
			}

			return QSizeF();
		};

		/* Render and return a page */
		QImage renderPage( int pgNo ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->renderPage( pgNo );
			}

			else {

				return popplerDoc->renderPage( pgNo );
			}

			return QImage();
		};

		/* Render and return a page */
		QImage renderPageForWidth( int pgNo, qreal width ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->renderPageForWidth( pgNo, width );
			}

			else {

				return popplerDoc->renderPageForWidth( pgNo, width );
			}

			return QImage();
		};

		/* Render and return a page */
		QImage renderPageForHeight( int pgNo, qreal height ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->renderPageForHeight( pgNo, height );
			}

			else {

				return popplerDoc->renderPageForHeight( pgNo, height );
			}

			return QImage();
		};

		/* Page Text */
		QString pageText( int pgNo ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->pageText( pgNo );
			}

			else {

				return popplerDoc->pageText( pgNo );
			}

			return QString();
		};

		/* Text of a Selection rectangle */
		QString text( int pgNo, QRectF rect ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->text( pgNo, rect );
			}

			else {

				return popplerDoc->text( pgNo, rect );
			}

			return QString();
		};

		qreal zoomForWidth( int pageNo, qreal width ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->zoomForWidth( pageNo, width );
			}

			else {

				return popplerDoc->zoomForWidth( pageNo, width );
			}

			return 1.0;
		};

		qreal zoomForHeight( int pageNo, qreal width ) const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->zoomForHeight( pageNo, width );
			}

			else {

				return popplerDoc->zoomForHeight( pageNo, width );
			}

			return 1.0;
		};

		/* Pdf scale factor */
		void setZoom( qreal zoom ) {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->setZoom( zoom );
			}

			else {

				return popplerDoc->setZoom( zoom );
			}
		};

		bool isReady() const {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				return muDoc->isReady();
			}

			else {

				return popplerDoc->isReady();
			}

			return false;
		}

	public Q_SLOTS:
		void loadDocument() {

			if ( mRenderBackend == MuPdfRenderBackend ) {

				muDoc->loadDocument();
			}

			else {

				popplerDoc->loadDocument();
			}
		};

	private:
		MuPdfDocument *muDoc;
		PopplerDocument *popplerDoc;

		int mRenderBackend;

	Q_SIGNALS:
		void pdfLoaded();
		void loadFailed();
};

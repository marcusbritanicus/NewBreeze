/*
	*
	* MuPdfDocument.hpp - MuPDF Document Qt interface header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#include "PdfDocument.hpp"

extern "C" {
	#include "mupdf/fitz.h"
}

typedef fz_page MuPage;

typedef QList<MuPage*> MuPages;

class MuPdfDocument : public PdfDocument {
	Q_OBJECT

	public:
		MuPdfDocument( QString );

		bool passwordNeeded() const;
		void setPassword( QString password );

		QString pdfName() const;
		QString pdfPath() const;

		int pageCount() const;
		QSizeF pageSize( int pageNo ) const;

		void reload();

		QImage renderPage( int ) const;
		QImage renderPageForWidth( int, qreal ) const;
		QImage renderPageForHeight( int, qreal ) const;

		QString pageText( int pageNo ) const;
		QString text( int pageNo, QRectF ) const;

		qreal zoomForWidth( int pageNo, qreal width ) const;
		qreal zoomForHeight( int pageNo, qreal width ) const;

		qreal zoom() const {

			return mZoom;
		};

		void setZoom( qreal zoom ) {

			mZoom = zoom;
		};

		bool isReady() const {

			return mLoaded;
		}

	private:
		QString mPdfPath;

		fz_context *mCtx;
		fz_document *mFzDoc;

		int mPages;
		MuPages mPageList;

		qreal mZoom;

		bool mLoaded;
		bool mPassNeeded;

	public Q_SLOTS:
		void loadDocument();
};

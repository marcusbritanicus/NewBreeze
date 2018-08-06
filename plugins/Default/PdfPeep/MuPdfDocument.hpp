/*
	*
	* MuPdfDocument.hpp - MuPDF Document Qt interface header
	*
*/

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

extern "C" {
	#include <mupdf/fitz.h>
}

typedef struct {
	fz_context *mCtx;
	fz_document *mFzDoc;
} MuDocument;

typedef fz_page MuPage;

typedef QList<MuPage*> MuPages;

class MuPdfDocument : public QObject {
	Q_OBJECT

	public:
		MuPdfDocument( QString );

		bool passwordNeeded();
		void setPassword( QString password );

		QString pdfName() const;
		QString pdfPath() const;

		MuDocument* document() {

			memcpy( mDocument->mCtx, mCtx, sizeof( fz_context ) );
			memcpy( mDocument->mFzDoc, mFzDoc, sizeof( fz_document ) );

			return mDocument;
		};

		fz_page* page( int pageNo ) const;

		/* MuPdf Pages */
		MuPages allPages() {

			return mPageList;
		};

		int pageCount() const;
		QSizeF pageSize( int pageNo ) const;

		QImage renderPage( int );

		QString pageText( int pageNo ) const;
		QString text( int pageNo, QRectF ) const;

		qreal zoomForWidth( int pageNo, qreal width );
		qreal zoomForHeight( int pageNo, qreal width );

		void setZoom( qreal zoom ) {

			mZoom = zoom;
		};

		bool isReady() const {

			return mLoaded;
		}

	private:
		QString mPdfPath;

		MuDocument *mDocument;

		fz_context *mCtx;
		fz_document *mFzDoc;

		int mPages;
		MuPages mPageList;

		qreal mZoom;

		bool mLoaded;
		bool mPassNeeded;

	public Q_SLOTS:
		void loadDocument();

	Q_SIGNALS:
		void pdfLoaded();
		void loadFailed();
};

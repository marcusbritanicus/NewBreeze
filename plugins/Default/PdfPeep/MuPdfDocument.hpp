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

		int pageCount() const;

		QSizeF pageSize( int pageNo ) const;
		QImage renderPageForWidth( int, qreal );

		qreal zoomForWidth( int pageNo, qreal width );

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

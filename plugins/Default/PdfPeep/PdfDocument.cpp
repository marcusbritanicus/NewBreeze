/*
	*
	* PdfDocument - Pdf Document Model for PdfWidget
	*
*/

#include "PdfDocument.hpp"

PdfDocument::PdfDocument( QString pdfPath ) : QObject() {

	mPdfPath = pdfPath;
	loadPdf();
};

Poppler::Document* PdfDocument::document() {

	return mPdfDoc;
};

PdfPages PdfDocument::allPages() {

	return mPages;
};

QString PdfDocument::name() const {

	return mPdfPath;
};

QString PdfDocument::title() const {

	return mPdfPath;
};

int PdfDocument::pages() const {

	return mPages.count();
};

PdfPage* PdfDocument::page( int pageNum ) const {

	return mPages.at( pageNum );
};

void PdfDocument::loadPdf() {

	mPdfDoc = Poppler::Document::load( mPdfPath );
	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	mPdfDoc->setRenderBackend( Poppler::Document::SplashBackend );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		PdfPage *p = mPdfDoc->page( i );
		mPages.append( p );
	}

	emit pdfLoaded();
};

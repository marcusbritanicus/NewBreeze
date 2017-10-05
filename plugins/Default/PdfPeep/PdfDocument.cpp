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

	document = Poppler::Document::load( mPdfPath );
	document->setRenderHint( Poppler::Document::Antialiasing );
	document->setRenderHint( Poppler::Document::TextAntialiasing );
	document->setRenderHint( Poppler::Document::TextHinting );

	document->setRenderBackend( Poppler::Document::SplashBackend );

	for( int i = 0; i < document->numPages(); i++ ) {
		PdfPage *p = document->page( i );
		mPages.append( p );
	}

	emit pdfLoaded();
};

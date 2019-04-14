/*
	*
	* PopplerDocument - Pdf Document Model for PdfWidget
	*
*/

#include "PopplerDocument.hpp"

PopplerDocument::PopplerDocument( QString pdfPath ) : PdfDocument( pdfPath ) {

	mZoom = 1.0;
	mLoaded = false;
	mPassNeeded = false;

	mPdfPath = pdfPath;
};

bool PopplerDocument::passwordNeeded() const {

	return mPassNeeded;
};

void PopplerDocument::setPassword( QString password ) {

	if ( mPdfDoc->unlock( password.toLatin1(), password.toLatin1() ) ) {
		qDebug() << "Invalid password. Please try again.\n";
		mPassNeeded = true;
		emit loadFailed();

		return;
	}

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		PdfPage *p = mPdfDoc->page( i );
		mPages.append( p );
	}

	emit pdfLoaded();
	mLoaded = true;
};

QString PopplerDocument::pdfName() const {

	return QFileInfo( mPdfPath ).baseName();
};

QString PopplerDocument::pdfPath() const {

	return QFileInfo( mPdfPath ).absolutePath();
};

int PopplerDocument::pageCount() const {

	return mPages.count();
};

QSizeF PopplerDocument::pageSize( int pageNo ) const {

	return mPages.at( pageNo )->pageSizeF() * mZoom;
};

void PopplerDocument::reload() {

	mLoaded = false;
	mPassNeeded = false;
	mPages.clear();

	loadDocument();
};

QImage PopplerDocument::renderPage( int pageNo ) const {

	return mPages.at( pageNo )->renderToImage( 72 * mZoom, 72 * mZoom );
};

QImage PopplerDocument::renderPageForWidth( int pageNo, qreal width ) const {

	qreal zoom = zoomForWidth( pageNo, width );
	return mPages.at( pageNo )->renderToImage( 72 * zoom, 72 * zoom );
};

QImage PopplerDocument::renderPageForHeight( int pageNo, qreal height ) const {

	qreal zoom = zoomForHeight( pageNo, height );
	return mPages.at( pageNo )->renderToImage( 72 * zoom, 72 * zoom );
};

QString PopplerDocument::pageText( int pageNo ) const {

	return mPages.at( pageNo )->text( QRectF() );
};

QString PopplerDocument::text( int pageNo, QRectF rect ) const {

	return mPages.at( pageNo )->text( rect );
};

qreal PopplerDocument::zoomForWidth( int pageNo, qreal width ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * width / mPages.at( pageNo )->pageSizeF().width();
};

qreal PopplerDocument::zoomForHeight( int pageNo, qreal height ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * height / mPages.at( pageNo )->pageSizeF().height();
};

void PopplerDocument::loadDocument() {

	mPdfDoc = Poppler::Document::load( mPdfPath );

	if ( not mPdfDoc ) {
		emit loadFailed();
		return;
	}

	if ( mPdfDoc->isLocked() ) {
		mPassNeeded = true;
		emit loadFailed();
		return;
	}

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		PdfPage *p = mPdfDoc->page( i );
		mPages.append( p );
	}

	emit pdfLoaded();
	mLoaded = true;
};

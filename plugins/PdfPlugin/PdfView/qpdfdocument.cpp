/*
    *
    * This file is a part of CorePDF.
    * A PDF viewer for CuboCore Application Suite
    * Copyright 2019 CuboCore Group
    *

    *
    * This program is free software; you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation; either version 3 of the License, or
    * (at your option) any later version.
    *

    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    *

    *
    * You should have received a copy of the GNU General Public License
    * along with this program; if not, vsit http://www.gnu.org/licenses/.
    *
*/

#include "qpdfdocument.h"

QPdfDocument::QPdfDocument( QString pdfPath ) : QObject() {

	mZoom = 1.0;
	mStatus = Null;
	mPassNeeded = false;

	mPdfPath = pdfPath;
};

bool QPdfDocument::passwordNeeded() const {

	return mPassNeeded;
};

void QPdfDocument::setPassword( QString password ) {

	if ( mPdfDoc->unlock( password.toLatin1(), password.toLatin1() ) ) {
		qDebug() << "Invalid password. Please try again.";
		mPassNeeded = true;
		emit statusChanged( Error );
		emit passwordRequired();

		return;
	}

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		PdfPage *p = mPdfDoc->page( i );
		mPages.append( p );
	}

	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
};

QString QPdfDocument::pdfName() const {

	return QFileInfo( mPdfPath ).fileName();
};

QString QPdfDocument::pdfPath() const {

	return QFileInfo( mPdfPath ).absolutePath();
};

QString QPdfDocument::title() const {

	return mPdfDoc->title();
};

QString QPdfDocument::author() const {

	return mPdfDoc->author();
};

QString QPdfDocument::creator() const {

	return mPdfDoc->creator();
};

QString QPdfDocument::producer() const {

	return mPdfDoc->producer();
};

QString QPdfDocument::created() const {

	return mPdfDoc->creationDate().toString( "MMM DD, yyyy hh:mm:ss t AP" );
};

int QPdfDocument::pageCount() const {

	return mPages.count();
};

QSizeF QPdfDocument::pageSize( int pageNo ) const {

	return mPages.at( pageNo )->pageSizeF() * mZoom;
};

void QPdfDocument::reload() {

	mStatus = Null;
	mPassNeeded = false;
	mPages.clear();

	load();
};

QPdfDocument::Status QPdfDocument::status() const {

	return mStatus;
};

QImage QPdfDocument::renderPage( int pageNo, QSize pSize, QPdfDocumentRenderOptions opts ) const {

	qreal wZoom = zoomForWidth( pageNo, pSize.width() );
	qreal hZoom = zoomForHeight( pageNo, pSize.height() );

	return mPages.at( pageNo )->renderToImage( 72 * wZoom, 72 * hZoom, -1, -1, -1, -1, ( Poppler::Page::Rotation )opts.rotation() );
};

QImage QPdfDocument::renderPage( int pageNo, qreal zoom, QPdfDocumentRenderOptions opts ) const {

	return mPages.at( pageNo )->renderToImage( 72 * zoom, 72 * zoom, -1, -1, -1, -1, ( Poppler::Page::Rotation )opts.rotation() );
};

QString QPdfDocument::pageText( int pageNo ) const {

	return mPages.at( pageNo )->text( QRectF() );
};

QString QPdfDocument::text( int pageNo, QRectF rect ) const {

	return mPages.at( pageNo )->text( rect );
};

qreal QPdfDocument::zoomForWidth( int pageNo, qreal width ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * width / mPages.at( pageNo )->pageSizeF().width();
};

qreal QPdfDocument::zoomForHeight( int pageNo, qreal height ) const {

	if ( pageNo >= mPages.count() )
		return 0.0;

	return 1.0 * height / mPages.at( pageNo )->pageSizeF().height();
};

void QPdfDocument::load() {

	mStatus = Loading;
	emit statusChanged( Loading );

	mPdfDoc = Poppler::Document::load( mPdfPath );

	if ( not mPdfDoc ) {
		mStatus = Error;
		qDebug() << "Poppler::Document load failed";
		emit statusChanged( Error );
		return;
	}

	if ( mPdfDoc->isLocked() ) {
		mStatus = Error;
		qDebug() << "Poppler::Document is locked";
		mPassNeeded = true;
		emit passwordRequired();
		emit statusChanged( Error );
		return;
	}

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		PdfPage *p = mPdfDoc->page( i );
		mPages.append( p );
	}

	mStatus = Ready;
	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
};

void QPdfDocument::close() {

	mStatus = Unloading;
	mPages.clear();
	mZoom = 1.0;

	delete mPdfDoc;
};

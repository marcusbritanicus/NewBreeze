/*
    *
    * This file is a part of DesQDocs.
    * DesQDocs is the default document viewer for the DesQ Suite
    * Copyright 2019-2021 Britanicus <marcusbritanicus@gmail.com>
    *

    *
    * This program is free software; you can redistribute it and/or modify
    * it under the terms of the GNU General Public License as published by
    * the Free Software Foundation; either version 2 of the License, or
    * at your option, any later version.
    *

    *
    * This program is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    * GNU General Public License for more details.
    *

    *
    * You should have received a copy of the GNU General Public License
    * along with this program; if not, write to the Free Software
    * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    * MA 02110-1301, USA.
    *
*/

#include "PopplerDocument.hpp"

PopplerDocument::PopplerDocument( QString pdfPath ) : DesQDocs::Document( pdfPath ) {

	mPdfDoc = nullptr;
};

void PopplerDocument::setPassword( QString password ) {

	if ( mPdfDoc->unlock( password.toLatin1(), password.toLatin1() ) ) {

		mStatus = Error;
		mDocError = IncorrectPasswordError;

		qDebug() << "Invalid password. Please try again.";
		mPassNeeded = true;
		emit statusChanged( Error );
		emit passwordRequired();

		return;
	}

	mPassNeeded = false;
	mStatus = Loading;
	mDocError = NoError;

	emit statusChanged( Loading );

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );

	for( int i = 0; i < mPdfDoc->numPages(); i++ ) {
		Poppler::Page *p = mPdfDoc->page( i );
		PdfPage *page = new PdfPage( i );
		page->setPageData( p );
		mPages.append( page );

		emit loading( 1.0 * i / mPdfDoc->numPages() * 100.0 );
	}

	mStatus = Ready;
	mDocError = NoError;

	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
	emit loading( 100 );
};

QString PopplerDocument::title() const {

	return mPdfDoc->title();
};

QString PopplerDocument::author() const {

	return mPdfDoc->author();
};

QString PopplerDocument::creator() const {

	return mPdfDoc->creator();
};

QString PopplerDocument::producer() const {

	return mPdfDoc->producer();
};

QString PopplerDocument::created() const {

	return mPdfDoc->creationDate().toString( "MMM DD, yyyy hh:mm:ss t AP" );
};

void PopplerDocument::load() {

	mStatus = Loading;
	emit statusChanged( Loading );

	if ( not QFile::exists( mDocPath ) ) {
		mStatus = Error;
		mDocError = FileNotFoundError;
		emit statusChanged( Error );

		return;
	}

	mPdfDoc = Poppler::Document::load( mDocPath );

	if ( not mPdfDoc ) {
		mStatus = Error;
		mDocError = UnknownError;
		qDebug() << "Poppler::Document load failed";
		emit statusChanged( Error );

		return;
	}

	if ( mPdfDoc->isLocked() ) {
		mStatus = Error;
		mDocError = IncorrectPasswordError;
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
		Poppler::Page *p = mPdfDoc->page( i );
		PdfPage *page = new PdfPage( i );
		page->setPageData( p );
		mPages.append( page );

		emit loading( 1.0 * i / mPdfDoc->numPages() * 100.0 );
	}

	mStatus = Ready;
	mDocError = NoError;

	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
	emit loading( 100 );
};

void PopplerDocument::close() {

	mStatus = Unloading;
	mPages.clear();
	mZoom = 1.0;

	delete mPdfDoc;
};


PdfPage::PdfPage( int pgNo ) : Page( pgNo ) {

	// Nothing much to be done here
};

PdfPage::~PdfPage() {

	delete m_page;
};

void PdfPage::setPageData( void *data ) {

	m_page = static_cast<Poppler::Page *>( data );
};

QSizeF PdfPage::pageSize( qreal zoom ) const {

	return m_page->pageSizeF() * zoom;
};

QImage PdfPage::thumbnail() const {

	return m_page->thumbnail();
};

QImage PdfPage::render( QSize pSize, DesQDocs::RenderOptions opts ) const {

	qreal wZoom = 1.0 * pSize.width() / m_page->pageSizeF().width();
	qreal hZoom = 1.0 * pSize.height() / m_page->pageSizeF().height();

	return m_page->renderToImage( 72 * wZoom, 72 * hZoom, -1, -1, -1, -1, ( Poppler::Page::Rotation )opts.rotation() );
};

QImage PdfPage::render( qreal zoomFactor, DesQDocs::RenderOptions opts ) const {

	return m_page->renderToImage( 72 * zoomFactor, 72 * zoomFactor, -1, -1, -1, -1, ( Poppler::Page::Rotation )opts.rotation() );
};

QString PdfPage::pageText() const {

	return text( QRectF() );
};

QString PdfPage::text( QRectF rect ) const {

	return m_page->text( rect );
};

QList<QRectF> PdfPage::search( QString query, DesQDocs::RenderOptions opts ) const {

	return m_page->search(
		query,																						// Search text
		Poppler::Page::IgnoreCase | Poppler::Page::IgnoreDiacritics,								// Case insensitive
		( Poppler::Page::Rotation )opts.rotation()													// Rotation
	);
};

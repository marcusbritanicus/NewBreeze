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

#include "MuPdfDocument.hpp"

MuPdfDocument::MuPdfDocument( QString pdfPath ) : DesQDocs::Document( pdfPath ) {

	mCtx = nullptr;
	mFzDoc = nullptr;
};

void MuPdfDocument::setPassword( QString password ) {

	if ( not fz_authenticate_password( mCtx, mFzDoc, password.toUtf8().constData() ) ) {
		fprintf( stderr, "Invalid password. Please try again.\n" );
		mPassNeeded = true;
		emit statusChanged( Error );

		return;
	}

	mPassNeeded = false;

	/** Count the number of pages. */
	fz_try( mCtx ) {
		mPageCount = fz_count_pages( mCtx, mFzDoc );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot count number of pages: %s\n", fz_caught_message( mCtx ) );

		fz_drop_document( mCtx, mFzDoc );
		fz_drop_context( mCtx );

		emit statusChanged( Error );
		return;
	}
	qDebug() << "=> Get page count" << mPageCount;

	for( int i = 0; i < mPageCount; i++ ) {
		fz_page *pg;

		fz_try( mCtx ) {

			pg = fz_load_page( mCtx, mFzDoc, i );
		}

		fz_catch( mCtx ) {
			fprintf( stderr, "Cannot create page: %s\n", fz_caught_message( mCtx ) );

			fz_drop_document( mCtx, mFzDoc );
			fz_drop_page( mCtx, pg );
			fz_drop_context( mCtx );

			emit statusChanged( Error );
			return;
		}

		/** Create MuPage isntance */
		MuPage *page = new MuPage( mCtx, pg, i );
		mPages << page;

		/** Drop reference to pg */
		fz_drop_page( mCtx, pg );

		emit loading( 1.0 * i / mPageCount * 100.0 );
	}

	mStatus = Ready;
	mDocError = NoError;

	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
	emit loading( 100 );
};

QString MuPdfDocument::title() const {

	char buf[1024] = { 0 };
	fz_lookup_metadata( mCtx, mFzDoc, "info:Title", buf, 1024 );

	return buf;
};

QString MuPdfDocument::author() const {

	char buf[1024] = { 0 };
	fz_lookup_metadata( mCtx, mFzDoc, "info:Author", buf, 1024 );

	return buf;
};

QString MuPdfDocument::creator() const {

	char buf[1024] = { 0 };
	fz_lookup_metadata( mCtx, mFzDoc, "info:Creator", buf, 1024 );

	return buf;
};

QString MuPdfDocument::producer() const {

	char buf[1024] = { 0 };
	fz_lookup_metadata( mCtx, mFzDoc, "info:Producer", buf, 1024 );

	return buf;
};

QString MuPdfDocument::created() const {

	char buf[1024] = { 0 };
	fz_lookup_metadata( mCtx, mFzDoc, "info:CreationDate", buf, 1024 );

	return buf;
};

void MuPdfDocument::load() {

	/** Create context */
	qDebug() << "=> Create context";
	mCtx = fz_new_context( NULL, NULL, FZ_STORE_UNLIMITED );
	if ( not mCtx ) {
		fprintf( stderr, "Cannot create mupdf context\n" );
		emit statusChanged( Error );
		return;
	}

	/** Register the default file types to handle. */
	fz_try( mCtx ){

		fz_register_document_handlers( mCtx );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot register document handlers: %s\n", fz_caught_message( mCtx ) ) ;

		fz_drop_context( mCtx );

		emit statusChanged( Error );
		return;
	}

	/** Open the document. */
	qDebug() << "=> Create document";
	fz_try( mCtx ) {
		mFzDoc = fz_open_document( mCtx, mDocPath.toUtf8().constData() );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot open document: %s\n", fz_caught_message( mCtx ) );

		fz_drop_context( mCtx );

		emit statusChanged( Error );
		return;
	}

	/** Check if the document is encrypted */
	qDebug() << "=> Needs password?";
	if ( fz_needs_password( mCtx, mFzDoc ) ) {
		fprintf( stderr, "Cannot load encrypted document\n" );

		fz_drop_document( mCtx, mFzDoc );
		fz_drop_context( mCtx );

		mStatus = Error;
		mDocError = IncorrectPasswordError;
		mPassNeeded = true;

		emit passwordRequired();
		emit statusChanged( Error );

		return;
	}

	/** Count the number of pages. */
	fz_try( mCtx ) {
		mPageCount = fz_count_pages( mCtx, mFzDoc );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot count number of pages: %s\n", fz_caught_message( mCtx ) );

		fz_drop_document( mCtx, mFzDoc );
		fz_drop_context( mCtx );

		emit statusChanged( Error );
		return;
	}
	qDebug() << "=> Get page count" << mPageCount;

	qDebug() << "=> Loading pages" << mPageCount;
	for( int i = 0; i < mPageCount; i++ ) {
		fz_page *pg;

		fz_try( mCtx ) {

			pg = fz_load_page( mCtx, mFzDoc, i );
		}

		fz_catch( mCtx ) {
			fprintf( stderr, "Cannot create page: %s\n", fz_caught_message( mCtx ) );

			fz_drop_document( mCtx, mFzDoc );
			fz_drop_page( mCtx, pg );
			fz_drop_context( mCtx );

			emit statusChanged( Error );
			return;
		}

		/** Create MuPage isntance */
		MuPage *page = new MuPage( mCtx, pg, i );
		mPages << page;

		/** Drop reference to pg */
		fz_drop_page( mCtx, pg );

		emit loading( 1.0 * i / mPageCount * 100.0 );
	}

	mStatus = Ready;
	mDocError = NoError;

	emit statusChanged( Ready );
	emit pageCountChanged( mPages.count() );
	emit loading( 100 );
};

void MuPdfDocument::close() {

	mStatus = Unloading;
	mPages.clear();
	mZoom = 1.0;

	fz_drop_document( mCtx, mFzDoc );
	fz_drop_context( mCtx );
};


MuPage::MuPage( fz_context *ctx, fz_page *pg, int pgNo ) : Page( pgNo ) {

	mCtx = ctx;
	mPage = fz_keep_page( ctx, pg );
};

MuPage::~MuPage() {

	fz_drop_page( mCtx, mPage );
	fz_drop_context( mCtx );
};

QSizeF MuPage::pageSize( qreal zoom ) const {

	fz_rect pgBox = fz_bound_page( mCtx, mPage );
	return QSizeF( pgBox.x1 - pgBox.x0, pgBox.y1 - pgBox.y0 ) * zoom;
};

QImage MuPage::thumbnail() const {

	// return mPage->thumbnail();
	return QImage();
};

QImage MuPage::render( QSize pSize, DesQDocs::RenderOptions opts ) const {

	qreal wZoom = 1.0 * pSize.width() / pageSize().width();
	qreal hZoom = 1.0 * pSize.height() / pageSize().height();

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

    colorspace = fz_device_bgr( mCtx );

	/** Otherwise, the page renders incorrectly */
	qreal rwZoom = pow( wZoom, 0.5 );
	qreal rhZoom = pow( hZoom, 0.5 );

	/** Setup the rotation */
	switch( ( DesQDocs::RenderOptions::Rotation )opts.rotation() ) {
		case DesQDocs::RenderOptions::Rotate0:
			mMtx = fz_rotate( 0 );
			break;

		case DesQDocs::RenderOptions::Rotate90:
			mMtx = fz_rotate( 90 );
			break;

		case DesQDocs::RenderOptions::Rotate180:
			mMtx = fz_rotate( 180 );
			break;

		case DesQDocs::RenderOptions::Rotate270:
			mMtx = fz_rotate( 270 );
			break;
	}

	mMtx = fz_scale( rwZoom, rhZoom );

	rBox = fz_bound_page( mCtx, mPage );
	iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
	rBox = fz_rect_from_irect( iBox );

	/** Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= rwZoom;
	iBox.y1 *= rhZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		fz_run_page( mCtx, mPage, dev, mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, mPage );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );

	return img;
};

QImage MuPage::render( qreal zoomFactor, DesQDocs::RenderOptions opts ) const {

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

    colorspace = fz_device_bgr( mCtx );

	/** Otherwise, the page renders incorrectly */
	qreal rwZoom = pow( zoomFactor, 0.5 );
	qreal rhZoom = pow( zoomFactor, 0.5 );

	/** Setup the rotation */
	switch( ( DesQDocs::RenderOptions::Rotation )opts.rotation() ) {
		case DesQDocs::RenderOptions::Rotate0:
			mMtx = fz_rotate( 0 );
			break;

		case DesQDocs::RenderOptions::Rotate90:
			mMtx = fz_rotate( 90 );
			break;

		case DesQDocs::RenderOptions::Rotate180:
			mMtx = fz_rotate( 180 );
			break;

		case DesQDocs::RenderOptions::Rotate270:
			mMtx = fz_rotate( 270 );
			break;
	}

	mMtx = fz_scale( rwZoom, rhZoom );

	rBox = fz_bound_page( mCtx, mPage );
	iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
	rBox = fz_rect_from_irect( iBox );

	/** Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= rwZoom;
	iBox.y1 *= rhZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		fz_run_page( mCtx, mPage, dev, mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, mPage );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );

	return img;
};

QString MuPage::pageText() const {

	QString ret;
    fz_stext_page *spg = NULL;
    fz_buffer *buff = NULL;
    fz_output *output = NULL;

	fz_try( mCtx ) {

		/** fz_stext_page */
		spg = fz_new_stext_page_from_page( mCtx, mPage, 0 );

		/** Buffer 1MiB = 1 * 1024 * 1024 */
		buff = fz_new_buffer( mCtx, 1024 * 1024 );

		/** Create fz_output */
		output = fz_new_output_with_buffer( mCtx, buff );

		/** Print it to text buffer */
		fz_print_stext_page_as_text( mCtx, output, spg );

		ret = QString::fromUtf8( fz_string_from_buffer( mCtx, buff ) );
	}

	fz_always( mCtx ) {

		fz_drop_buffer( mCtx, buff );
		fz_drop_output( mCtx, output );
		fz_drop_stext_page( mCtx, spg );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Unable to extract text from page: %s\n", fz_caught_message( mCtx ) );
	}

    return ret;
};

QString MuPage::text( QRectF rect ) const {

	QString ret;
	fz_stext_page *textPage;
    fz_point a, b;
    char *str;

	a.x = rect.left();
	a.y = rect.top();
	b.x = rect.right();
	b.y = rect.bottom();

	fz_try( mCtx ) {
		/** Structured text page */
    	textPage = fz_new_stext_page_from_page( mCtx, mPage, 0 );

		/** Copy the text from selection */
		str = fz_copy_selection( mCtx, textPage, a, b, 0);
	}

	fz_always( mCtx ) {
		ret = QString::fromUtf8( str );
		free( str );
		fz_drop_stext_page( mCtx, textPage );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Unable to extract text from page: %s\n", fz_caught_message( mCtx ) );
	}

    return ret;
};

QList<QRectF> MuPage::search( QString query, DesQDocs::RenderOptions opts ) const {

	QList<QRectF> ret;
    fz_stext_page *spg = NULL;
    fz_quad quads[ 1024 ] = { 0 };
	int count = 0;

	int fz_search_stext_page(fz_context *ctx, fz_stext_page *text, const char *needle, fz_quad *quads, int max_quads);

	fz_try( mCtx ) {

		/** fz_stext_page */
		spg = fz_new_stext_page_from_page( mCtx, mPage, 0 );

		/** Search the text in this page */
		count = fz_search_stext_page( mCtx, spg, query.toUtf8().data(), quads, 1024 );
	}

	fz_always( mCtx ) {

		fz_drop_stext_page( mCtx, spg );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Unable to search text in page: %s\n", fz_caught_message( mCtx ) );
	}

    return ret;
};

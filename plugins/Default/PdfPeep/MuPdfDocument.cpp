﻿/*
	*
	* MuPdfDocument.cpp - MuPDF Document Qt interface
	*
*/

#include "MuPdfDocument.hpp"

MuPdfDocument::MuPdfDocument( QString pdfPath ) {

	mZoom = 1.0;

	mLoaded = false;
	mPassNeeded = false;
	mPages = 0;

	mPdfPath = QString( pdfPath );
};

bool MuPdfDocument::passwordNeeded() {

	return mPassNeeded;
};

void MuPdfDocument::setPassword( QString password ) {

	if ( not fz_authenticate_password( mCtx, mFzDoc, password.toUtf8().constData() ) ) {
		fprintf( stderr, "Invalid apssword. Please try again.\n" );
		mPassNeeded = true;
		emit loadFailed();
	}

	loadDocument();
};

QString MuPdfDocument::pdfName() const {

	return QFileInfo( mPdfPath ).baseName();
};

QString MuPdfDocument::pdfPath() const {

	return QFileInfo( mPdfPath ).absolutePath();
};

int MuPdfDocument::pageCount() const {

	return mPages;
};

QSizeF MuPdfDocument::pageSize( int pageNum ) const {

	if ( pageNum >= mPages )
		return QSizeF();

	fz_rect rBox;
	fz_bound_page( mCtx, mPageList.at( pageNum ), &rBox );

	return QSizeF( mZoom * ( rBox.x1 - rBox.x0 ), mZoom * ( rBox.y1 - rBox.y0 ) );
};

QImage MuPdfDocument::renderPageForWidth( int pgNo, qreal width ) {

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

	colorspace = fz_device_bgr( mCtx );

	MuPage *pg = mPageList.at( pgNo );

	qreal realZoom = pow( zoomForWidth( pgNo, width ), 0.5 );

	fz_rotate( &mMtx, 0 );
	fz_scale( &mMtx, realZoom, realZoom );

	fz_bound_page( mCtx, pg, &rBox );
	fz_round_rect( &iBox, fz_transform_rect( &rBox, &mMtx ) );
	fz_rect_from_irect( &rBox, &iBox );

	/* Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= realZoom;
	iBox.y1 *= realZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, &iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, &mMtx, image, &iBox );

		fz_run_page( mCtx, pg, dev, &mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, pg );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	unsigned char *argb32 = new unsigned char[ ( iBox.x1 - iBox.x0 ) * ( iBox.y1 - iBox.y0 ) ];
	unsigned char *rgba32 = image->samples;

	size_t count = ( iBox.x1 - iBox.x0 ) * ( iBox.y1 - iBox.y0 );

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );
	return img;
};

qreal MuPdfDocument::zoomForWidth( int pageNo, qreal width ) {

	if ( pageNo >= mPages )
		return 0.0;

	fz_rect rBox;
	fz_bound_page( mCtx, mPageList.at( pageNo ), &rBox );

	return 1.0 * width / ( rBox.x1 - rBox.x0 );
};

void MuPdfDocument::loadDocument() {

	/* Create context */
	mCtx = fz_new_context( NULL, NULL, FZ_STORE_UNLIMITED );
	if ( not mCtx ) {
		fprintf( stderr, "Cannot create mupdf context\n" );
		emit loadFailed();
		return;
	}

	/* Register the default file types to handle. */
	fz_try( mCtx ){

		fz_register_document_handlers( mCtx );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot register document handlers: %s\n", fz_caught_message( mCtx ) ) ;

		fz_drop_context( mCtx );

		emit loadFailed();
		return;
	}

	/* Open the document. */
	fz_try( mCtx ) {
		mFzDoc = fz_open_document( mCtx, mPdfPath.toUtf8().constData() );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot open document: %s\n", fz_caught_message( mCtx ) );

		fz_drop_context( mCtx );

		emit loadFailed();
		return;
	}

	/* Count the number of pages. */
	fz_try( mCtx ) {
		mPages = fz_count_pages( mCtx, mFzDoc );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot count number of pages: %s\n", fz_caught_message( mCtx ) );

		fz_drop_document( mCtx, mFzDoc );
		fz_drop_context( mCtx );

		emit loadFailed();
		return;
	}

	/* Check if the document is encrypted */
	if ( fz_needs_password( mCtx, mFzDoc ) ) {
		fprintf( stderr, "Cannot load encrypted document\n" );

		fz_drop_document( mCtx, mFzDoc );
		fz_drop_context( mCtx );

		mPassNeeded = true;
		emit loadFailed();
		return;
	}

	for( int i = 0; i < mPages; i++ ) {
		fz_page *page = 0;

		fz_try( mCtx )
			page = fz_load_page( mCtx, mFzDoc, i );

		fz_catch( mCtx ) {
			fprintf( stderr, "Cannot create page: %s\n", fz_caught_message( mCtx ) );

			fz_drop_document( mCtx, mFzDoc );
			fz_drop_page( mCtx, page );
			fz_drop_context( mCtx );

			emit loadFailed();
			return;
		}

		mPageList << page;
	}

	emit pdfLoaded();
	mLoaded = true;
};

/*
	*
	* MuPdfDocument.cpp - MuPDF Document Qt interface
	*
*/

#include "MuPdfDocument.hpp"

MuPdfDocument::MuPdfDocument( QString pdfPath ) : PdfDocument( pdfPath ) {

	mZoom = 1.0;

	mLoaded = false;
	mPassNeeded = false;
	mPages = 0;

	mPdfPath = QString( pdfPath );
};

bool MuPdfDocument::passwordNeeded() const {

	return mPassNeeded;
};

void MuPdfDocument::setPassword( QString password ) {

	if ( not fz_authenticate_password( mCtx, mFzDoc, password.toUtf8().constData() ) ) {
		fprintf( stderr, "Invalid password. Please try again.\n" );
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
	rBox = fz_bound_page( mCtx, mPageList.at( pageNum ) );

	return QSizeF( mZoom * ( rBox.x1 - rBox.x0 ), mZoom * ( rBox.y1 - rBox.y0 ) );
};

void MuPdfDocument::reload() {

	mLoaded = false;
	mPassNeeded = false;
	mPages = 0;

	loadDocument();
};

QImage MuPdfDocument::renderPage( int pgNo ) const {

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

    colorspace = fz_device_bgr( mCtx );

	MuPage *pg = mPageList.at( pgNo );

	qreal realZoom = pow( mZoom, 0.5 );

	mMtx = fz_rotate( 0 );
	mMtx = fz_scale( realZoom, realZoom );

	rBox = fz_bound_page( mCtx, pg );
	iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
	rBox = fz_rect_from_irect( iBox );

	/* Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= realZoom;
	iBox.y1 *= realZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		fz_run_page( mCtx, pg, dev, mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, pg );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );
	return img;
};

QImage MuPdfDocument::renderPageForWidth( int pgNo, qreal width ) const {

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

    colorspace = fz_device_bgr( mCtx );

	MuPage *pg = mPageList.at( pgNo );

	qreal realZoom = pow( zoomForWidth( pgNo, width ), 0.5 );

	mMtx = fz_rotate( 0 );
	mMtx = fz_scale( realZoom, realZoom );

	rBox = fz_bound_page( mCtx, pg );
	iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
	rBox = fz_rect_from_irect( iBox );

	/* Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= realZoom;
	iBox.y1 *= realZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		fz_run_page( mCtx, pg, dev, mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, pg );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );
	return img;
};

QImage MuPdfDocument::renderPageForHeight( int pgNo, qreal height ) const {

	fz_irect iBox;
	fz_rect rBox;
	fz_pixmap *image;
	fz_colorspace *colorspace;
	fz_matrix mMtx;

    colorspace = fz_device_bgr( mCtx );

	MuPage *pg = mPageList.at( pgNo );

	qreal realZoom = pow( zoomForHeight( pgNo, height ), 0.5 );

	mMtx = fz_rotate( 0 );
	mMtx = fz_scale( realZoom, realZoom );

	rBox = fz_bound_page( mCtx, pg );
	iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
	rBox = fz_rect_from_irect( iBox );

	/* Necessary: otherwise only a part of the page is rendered */
	iBox.x1 *= realZoom;
	iBox.y1 *= realZoom;

	fz_try( mCtx ) {
		image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		fz_run_page( mCtx, pg, dev, mMtx, NULL );
	}

	fz_catch( mCtx ) {

		fprintf( stderr, "Cannot render page: %s\n", fz_caught_message( mCtx ) ) ;
		fz_drop_page( mCtx, pg );
		fz_drop_colorspace( mCtx, colorspace );
		return QImage();
	}

	QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );
	return img;
};

QString MuPdfDocument::pageText( int pgNo ) const {

    QString ret;
    fz_stext_page *spg = NULL;
    fz_buffer *buff = NULL;
    fz_output *output = NULL;

	fz_try( mCtx ) {

		/* fz_stext_page */
		spg = fz_new_stext_page_from_page( mCtx, mPageList.at( pgNo ), 0 );

		/* Buffer 1MiB = 1 * 1024 * 1024 */
		buff = fz_new_buffer( mCtx, 1024 * 1024 );

		/* Create fz_output */
		output = fz_new_output_with_buffer( mCtx, buff );

		/* Print it to text buffer */
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

QString MuPdfDocument::text( int pgNo, QRectF rect ) const{

    QString ret;
    fz_point a, b;
    char *str;

    MuPage *pg = mPageList.at( pgNo );
    fz_stext_page *textPage = fz_new_stext_page_from_page( mCtx, pg, 0 );

	a.x = rect.left();
	a.y = rect.top();
	b.x = rect.right();
	b.y = rect.bottom();

	str = fz_copy_selection( mCtx, textPage, a, b, 0);
	ret = QString::fromUtf8( str );
	free( str );

	fz_drop_stext_page( mCtx, textPage );

    return ret;
};

qreal MuPdfDocument::zoomForWidth( int pageNo, qreal width ) const {

	if ( pageNo >= mPages )
		return 0.0;

	fz_rect rBox;
	rBox = fz_bound_page( mCtx, mPageList.at( pageNo ) );

	return 1.0 * width / ( rBox.x1 - rBox.x0 );
};

qreal MuPdfDocument::zoomForHeight( int pageNo, qreal height ) const {

	if ( pageNo >= mPages )
		return 0.0;

	fz_rect rBox;
	rBox = fz_bound_page( mCtx, mPageList.at( pageNo ) );

	return 1.0 * height / ( rBox.y1 - rBox.y0 );
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

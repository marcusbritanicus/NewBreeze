/*
	*
	* NBThumbnailerPlugin.cpp - NewBreeze Video Thumbnailer Class
	*
*/

#include "NBThumbnailerPlugin.hpp"

static QMimeDatabase mimeDb;

/* Name of the plugin */
QString NBThumbnailerPlugin::name() {

	return "Thumbnailer";
};

/* The plugin version */
QString NBThumbnailerPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBThumbnailerPlugin::actions( Interface, QStringList ) {

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBThumbnailerPlugin::interfaces() {

	return Interfaces() << NBPluginInterface::MimeTypeInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBThumbnailerPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBThumbnailerPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::None;
};

/* Mimetypes handled by the plugin */
QStringList NBThumbnailerPlugin::mimetypes() {

	QStringList mimeList;
	Q_FOREACH( QMimeType type, mimeDb.allMimeTypes() ) {

		if ( type.name().startsWith( "video" ) )
			mimeList << type.name();

		else if ( type.name().contains( "pdf" ) )
			mimeList << type.name();

		else if ( type.name().contains( "djvu" ) )
			mimeList << type.name();

		else if ( type.name().contains( "epub" ) )
			mimeList << type.name();
	}

	mimeList.removeDuplicates();
	mimeList.removeAll( "video/mng" );

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBThumbnailerPlugin::actionTrigger( Interface, QString fileType, QStringList nodes ) {
	/* @nodes[ 1 ] = file; @nodes[ 2 ] = hashPath */

	if ( fileType == "Video" )
		makeVideoThumbnail( nodes.at( 0 ), nodes.at( 1 ) );

	else if ( fileType == "DjVu" )
		makeDjVuThumbnail( nodes.at( 0 ), nodes.at( 1 ) );

	else if ( fileType == "ePub" )
		makeEpubThumbnail( nodes.at( 0 ), nodes.at( 1 ) );

	else if ( fileType == "PDF" )
		makePdfThumbnail( nodes.at( 0 ), nodes.at( 1 ) );
};

void NBThumbnailerPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

void NBThumbnailerPlugin::makeVideoThumbnail( QString vFile, QString hashPath ) {

	ffmpegthumbnailer::VideoThumbnailer *mThumbnailer = new ffmpegthumbnailer::VideoThumbnailer();
	mThumbnailer->setSeekPercentage( 33 );

	ffmpegthumbnailer::FilmStripFilter mFilmStrip;
	mThumbnailer->addFilter( &mFilmStrip );

	QImage thumb;

	try {
		std::vector<uint8_t> pixelBuffer;
		mThumbnailer->setThumbnailSize( 128 );
		mThumbnailer->generateThumbnail( vFile.toStdString(), Png, pixelBuffer );
		thumb.loadFromData( &pixelBuffer.front(), pixelBuffer.size(), "PNG" );
	}

	catch ( std::exception &e ) {
	}

	if ( thumb.isNull() or not thumb.save( hashPath, "png", 0 ) ) {

		QFile::copy( ":/icons/video.png", hashPath );
		qDebug() << "Failed to create thumbnail:" << baseName( vFile ) << "Using default icon.";
	}
};

void NBThumbnailerPlugin::makeDjVuThumbnail( QString path, QString hashPath ) {

	/* DjVu Context */
	ddjvu_context_t *ctx = ddjvu_context_create( "newbreeze3" );

	/* DjVu Document */
	ddjvu_document_t *doc = ddjvu_document_create_by_filename( ctx, path.toLocal8Bit().data(), 1 );

	/* Wait for decoding to be complete */
	ddjvu_job_t *job = ddjvu_document_job( doc );
	ddjvu_message_wait( ctx );
	while ( ddjvu_job_status( job ) < DDJVU_JOB_OK )
		continue;

	if ( ddjvu_job_status( job ) == DDJVU_JOB_FAILED ) {
		qWarning() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}

	/* DjVu page ptr */
	ddjvu_page_t *pg;
	/* DjVu page job ptr */
	ddjvu_job_t *pgjob;

	/* DjVu page info struct */
	ddjvu_pageinfo_t info;

	/* DjVu page render rect */
	ddjvu_rect_t rect;
	rect.w = 128;
	rect.x = 0;
	rect.y = 0;

	/* DjVu page render format */
	unsigned int masks[ 4 ] = { 0xff0000, 0xff00, 0xff, 0xff000000 };
	ddjvu_format_t *fmt = ddjvu_format_create( DDJVU_FORMAT_RGBMASK32, 4, masks );

	/* Create and decode page */
	pg = ddjvu_page_create_by_pageno( doc, 0 );
	pgjob = ddjvu_page_job( pg );

	while ( ddjvu_job_status( pgjob ) < DDJVU_JOB_OK )
		continue;

	while ( ddjvu_document_get_pageinfo_imp( doc, 0, &info, sizeof( ddjvu_pageinfo_t ) ) < DDJVU_JOB_OK )
		continue;

	/* Set rectange height */
	rect.h = 128 * info.height / info.width;

	/* Make DjVu decoder follow X11 conventions */
	ddjvu_format_set_row_order( fmt, true );
	ddjvu_format_set_y_direction( fmt, true );

	/* Render the DjVu page to QImage */
	QImage image( rect.w, rect.h, QImage::Format_RGB32 );
	ddjvu_page_render( pg, DDJVU_RENDER_COLOR, &rect, &rect, fmt, image.bytesPerLine(), (char*)image.bits() );

	QSize iSize = image.size();

	QImage thumb( 128, 128, QImage::Format_ARGB32 );
	thumb.fill( Qt::transparent );

	QPainter painter( &thumb );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	painter.drawImage( QRect( ( 128 - iSize.width() ) / 2, ( 128 - iSize.height() ) / 2, iSize.width(), iSize.height() ), image );

	painter.setPen( QPen( QColor( 44, 30, 85 ).lighter( 120 ), 3.0 ) );
	painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
	painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
	painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "DJVU" );

	painter.end();

	if ( not thumb.save( hashPath, "png", 0 ) )
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
};

void NBThumbnailerPlugin::makeEpubThumbnail( QString path, QString hashPath ) {

	fz_context *mCtx;
	fz_document *mFzDoc;

	/* Create context */
	mCtx = fz_new_context( NULL, NULL, FZ_STORE_UNLIMITED );
	if ( not mCtx ) {
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}

	/* Register the default file types to handle. */
	fz_try( mCtx ) {
		fz_register_document_handlers( mCtx );
		mFzDoc = fz_open_document( mCtx, path.toUtf8().constData() );

		/* Check if the document is encrypted */
		if ( fz_needs_password( mCtx, mFzDoc ) ) {
			fz_drop_context( mCtx );
			fz_drop_document( mCtx, mFzDoc );
			qDebug() << "Encrypted document:" << baseName( path ) << "Using default icon.";
			return;
		}

		int mPages = fz_count_pages( mCtx, mFzDoc );
		if ( not mPages ) {
			fz_drop_context( mCtx );
			fz_drop_document( mCtx, mFzDoc );
			qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
			return;
		}

		fz_page *page = fz_load_page( mCtx, mFzDoc, 0 );

		fz_irect iBox;
		fz_rect rBox;
		fz_pixmap *image;
		fz_colorspace *colorspace;
		fz_matrix mMtx;

		colorspace = fz_device_bgr( mCtx );
		fz_bound_page( mCtx, page, &rBox );

		int width = rBox.x1 - rBox.x0;
		int height = rBox.x1 - rBox.x0;

		qreal zoom;
		if ( width > height )
			zoom = pow( 128.0 / width, 0.5 );

		else
			zoom = pow( 128.0 / height, 0.5 );

		fz_rotate( &mMtx, 0 );
		fz_scale( &mMtx, zoom, zoom );

		fz_round_rect( &iBox, fz_transform_rect( &rBox, &mMtx ) );
		fz_rect_from_irect( &rBox, &iBox );

		/* Necessary: otherwise only a part of the page is rendered */
		iBox.x1 *= zoom;
		iBox.y1 *= zoom;

		image = fz_new_pixmap_with_bbox( mCtx, colorspace, &iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, &mMtx, image, &iBox );

		fz_run_page( mCtx, page, dev, &mMtx, NULL );

		QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );

		QImage thumb( 128, 128, QImage::Format_ARGB32 );
		thumb.fill( Qt::transparent );

		QPainter painter( &thumb );
		painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

		painter.drawImage( QRect( ( 128 - img.width() ) / 2, ( 128 - img.height() ) / 2, img.width(), img.height() ), img );

		painter.setPen( QPen( QColor( 44, 30, 85 ).lighter( 120 ), 3.0 ) );
		painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
		painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
		painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "EPUB" );

		painter.end();

		if ( not thumb.save( hashPath, "png", 0 ) )
			qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
	}

	fz_catch( mCtx ) {
		fz_drop_context( mCtx );
		fz_drop_document( mCtx, mFzDoc );
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}
};

void NBThumbnailerPlugin::makePdfThumbnail( QString path, QString hashPath ) {

	fz_context *mCtx;
	fz_document *mFzDoc;

	/* Create context */
	mCtx = fz_new_context( NULL, NULL, FZ_STORE_UNLIMITED );
	if ( not mCtx ) {
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}

	/* Register the default file types to handle. */
	fz_try( mCtx ) {
		fz_register_document_handlers( mCtx );
		mFzDoc = fz_open_document( mCtx, path.toUtf8().constData() );

		/* Check if the document is encrypted */
		if ( fz_needs_password( mCtx, mFzDoc ) ) {
			fz_drop_context( mCtx );
			fz_drop_document( mCtx, mFzDoc );
			qDebug() << "Encrypted document:" << baseName( path ) << "Using default icon.";
			return;
		}

		int mPages = fz_count_pages( mCtx, mFzDoc );
		if ( not mPages ) {
			fz_drop_context( mCtx );
			fz_drop_document( mCtx, mFzDoc );
			qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
			return;
		}

		fz_page *page = fz_load_page( mCtx, mFzDoc, 0 );

		fz_irect iBox;
		fz_rect rBox;
		fz_pixmap *image;
		fz_colorspace *colorspace;
		fz_matrix mMtx;

		colorspace = fz_device_bgr( mCtx );
		fz_bound_page( mCtx, page, &rBox );

		int width = rBox.x1 - rBox.x0;
		int height = rBox.x1 - rBox.x0;

		qreal zoom;
		if ( width > height )
			zoom = pow( 128.0 / width, 0.5 );

		else
			zoom = pow( 128.0 / height, 0.5 );

		fz_rotate( &mMtx, 0 );
		fz_scale( &mMtx, zoom, zoom );

		fz_round_rect( &iBox, fz_transform_rect( &rBox, &mMtx ) );
		fz_rect_from_irect( &rBox, &iBox );

		/* Necessary: otherwise only a part of the page is rendered */
		iBox.x1 *= zoom;
		iBox.y1 *= zoom;

		image = fz_new_pixmap_with_bbox( mCtx, colorspace, &iBox, 0, 1 );
		fz_clear_pixmap_with_value( mCtx, image, 0xff );
		fz_device *dev = fz_new_draw_device_with_bbox( mCtx, &mMtx, image, &iBox );

		fz_run_page( mCtx, page, dev, &mMtx, NULL );

		QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );

		QImage thumb( 128, 128, QImage::Format_ARGB32 );
		thumb.fill( Qt::transparent );

		QPainter painter( &thumb );
		painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

		painter.drawImage( QRect( ( 128 - img.width() ) / 2, ( 128 - img.height() ) / 2, img.width(), img.height() ), img );

		painter.setPen( QPen( QColor( 160, 0, 0 ), 3.0 ) );
		painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
		painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
		painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "PDF" );

		painter.end();

		if ( not thumb.save( hashPath, "png", 0 ) )
			qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
	}

	fz_catch( mCtx ) {
		fz_drop_context( mCtx );
		fz_drop_document( mCtx, mFzDoc );
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( Thumbnailer, NBThumbnailerPlugin );
#endif

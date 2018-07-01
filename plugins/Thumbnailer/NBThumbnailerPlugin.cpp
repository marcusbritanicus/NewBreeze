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

void NBThumbnailerPlugin::makePdfThumbnail( QString path, QString hashPath ) {

	Poppler::Document *mPdfDoc = Poppler::Document::load( path );

	if ( not mPdfDoc ) {
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		return;
	}

	mPdfDoc->setRenderHint( Poppler::Document::Antialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextAntialiasing );
	mPdfDoc->setRenderHint( Poppler::Document::TextHinting );
	mPdfDoc->setRenderBackend( Poppler::Document::SplashBackend );

	Poppler::Page *p = mPdfDoc->page( 0 );
	float res = 128 * 72 / p->pageSize().width();
	QImage image = p->renderToImage( res, res );

	QImage thumb( 128, 128, QImage::Format_ARGB32 );
	thumb.fill( Qt::transparent );

	QPainter painter( &thumb );
	painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	painter.drawImage( QRect( ( 128 - image.width() ) / 2, ( 128 - image.height() ) / 2, image.width(), image.height() ), image );

	painter.setPen( QPen( QColor( 160, 0, 0 ), 3.0 ) );
	painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
	painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
	painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "PDF" );

	painter.end();

	if ( not thumb.save( hashPath, "png", 0 ) )
		qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( Thumbnailer, NBThumbnailerPlugin );
#endif

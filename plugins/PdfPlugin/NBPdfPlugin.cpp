/*
	*
	* NBPdfPlugin.cpp - NewBreeze Video Thumbnailer Class
	*
*/

#include "NBPdfPlugin.hpp"

static QMimeDatabase mimeDb;

/* Name of the plugin */
QString NBPdfPlugin::name() {

	return "PdfPlugin";
};

/* The plugin version */
QString NBPdfPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBPdfPlugin::actions( Interface iface, QStringList nodes ) {

	QList<QAction*> acts;

	if ( ( nodes.count() != 1 ) or not isFile( nodes.at( 0 ) ) )
		return acts;

	QMimeType mime = mimeDb.mimeTypeForFile( nodes.at( 0 ) );
	if( iface == NBPluginInterface::PreviewInterface ) {
		if ( mime.name().contains( "pdf" ) or mime.name().contains( "epub" ) or mime.name().contains( "comicbook" ) ) {
			NBPdfPeep *peek = new NBPdfPeep( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );

			QAction *act = new QAction( QIcon( ":/icons/peek.png" ), "&Peek", this );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );

			acts << act;
		}
	};

	return acts;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBPdfPlugin::interfaces() {

	return Interfaces() << NBPluginInterface::MimeTypeInterface << NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBPdfPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBPdfPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList NBPdfPlugin::mimetypes() {

	QStringList mimeList;
	Q_FOREACH( QMimeType type, mimeDb.allMimeTypes() ) {

		if ( type.name().contains( "pdf" ) )
			mimeList << type.name();

		else if ( type.name().contains( "epub" ) )
			mimeList << type.name();

		else if ( type.name().contains( "comicbook" ) )
			mimeList << type.name();
	}

	mimeList.removeDuplicates();

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBPdfPlugin::actionTrigger( Interface iface, QString fileType, QStringList nodes ) {
	/* @nodes[ 1 ] = file; @nodes[ 2 ] = hashPath */

	if ( iface == MimeTypeInterface ) {
		QStringList allowed = QStringList() << "ePub" << "PDF" << "CBZ";
		if ( allowed.contains( fileType ) )
			makeThumbnail( nodes.at( 0 ), nodes.at( 1 ) );
	}

	else if( iface == NBPluginInterface::PreviewInterface ) {

		QString file = nodes.at( 0 );

		if ( not isFile( file ) )
			return;

		QMimeType mime = mimeDb.mimeTypeForFile( file );
		if ( mime.name().contains( "pdf" ) or mime.name().contains( "epub" ) or mime.name().contains( "comicbook" ) ) {
			NBPdfPeep *peek = new NBPdfPeep( file );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			peek->exec();
		}
	}
};

void NBPdfPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

void NBPdfPlugin::makeThumbnail( QString path, QString hashPath ) {

	PopplerDocument *doc = new PopplerDocument( path );
	doc->loadDocument();

	if ( doc->pageCount() ) {
		QImage pic = doc->renderPage( 0 );
		pic = pic.scaled( 512, 512, Qt::KeepAspectRatio, Qt::FastTransformation );
		pic = pic.scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		QPixmap thumb( QSize( 128, 128 ) );
		thumb.fill( Qt::transparent );

		QPainter painter( &thumb );
		painter.drawImage( QRectF( QPointF( ( 128 - pic.width() ) / 2, ( 128 - pic.height() ) / 2 ), QSizeF( pic.size() ) ), pic );
		painter.end();

		thumb.save( hashPath, "PNG" );
	}

	/* Below is MuPDF code */
	// if ( getSize( path ) == 0 ) {

		// qDebug() << "Empty document.";
		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// fz_context *mCtx;
	// fz_document *mFzDoc;

	// /* Create context */
	// mCtx = fz_new_context( NULL, NULL, FZ_STORE_UNLIMITED );
	// if ( not mCtx ) {

		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// /* Register the default file types to handle. */
	// fz_try( mCtx ){

		// fz_register_document_handlers( mCtx );
	// }

	// fz_catch( mCtx ) {

		// fz_drop_context( mCtx );

		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// /* Open the document. */
	// fz_try( mCtx ) {
		// mFzDoc = fz_open_document( mCtx, path.toUtf8().constData() );
	// }

	// fz_catch( mCtx ) {

		// fz_drop_context( mCtx );

		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// /* Count the number of pages. */
	// fz_try( mCtx ) {
		// int mPages = fz_count_pages( mCtx, mFzDoc );
		// if ( not mPages ) {

			// fz_drop_document( mCtx, mFzDoc );
			// fz_drop_context( mCtx );

			// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
			// return;
		// }
	// }

	// fz_catch( mCtx ) {

		// fz_drop_document( mCtx, mFzDoc );
		// fz_drop_context( mCtx );

		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// /* Check if the document is encrypted */
	// if ( fz_needs_password( mCtx, mFzDoc ) ) {

		// fz_drop_document( mCtx, mFzDoc );
		// fz_drop_context( mCtx );

		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }

	// fz_try( mCtx ) {

		// fz_page *page = fz_load_page( mCtx, mFzDoc, 0 );

		// fz_irect iBox;
		// fz_rect rBox;
		// fz_pixmap *image;
		// fz_colorspace *colorspace;
		// fz_matrix mMtx;

		// colorspace = fz_device_bgr( mCtx );
		// rBox = fz_bound_page( mCtx, page );

		// int width = rBox.x1 - rBox.x0;
		// int height = rBox.x1 - rBox.x0;

		// qreal zoom;
		// if ( width > height )
			// zoom = pow( 128.0 / width, 0.5 );

		// else
			// zoom = pow( 128.0 / height, 0.5 );

		// mMtx = fz_rotate( 0 );
		// mMtx = fz_scale( zoom, zoom );

		// iBox = fz_round_rect( fz_transform_rect( rBox, mMtx ) );
		// rBox = fz_rect_from_irect( iBox );

		// /* Necessary: otherwise only a part of the page is rendered */
		// iBox.x1 *= zoom;
		// iBox.y1 *= zoom;

		// image = fz_new_pixmap_with_bbox( mCtx, colorspace, iBox, 0, 1 );
		// fz_clear_pixmap_with_value( mCtx, image, 0xff );
		// fz_device *dev = fz_new_draw_device_with_bbox( mCtx, mMtx, image, &iBox );

		// fz_run_page( mCtx, page, dev, mMtx, NULL );

		// QImage img = QImage( image->samples, ( iBox.x1 - iBox.x0 ), ( iBox.y1 - iBox.y0 ), QImage::Format_ARGB32 );

		// QImage thumb( 128, 128, QImage::Format_ARGB32 );
		// thumb.fill( Qt::transparent );

		// QPainter painter( &thumb );
		// painter.setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

		// painter.drawImage( QRect( ( 128 - img.width() ) / 2, ( 128 - img.height() ) / 2, img.width(), img.height() ), img );

		// if ( path.endsWith( "pdf" ) ) {
			// painter.setPen( QPen( QColor( 160, 0, 0 ), 3.0 ) );
			// painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
			// painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
			// painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "PDF" );
		// }

		// else if ( path.endsWith( "epub" ) ) {
			// painter.setPen( QPen( QColor( 44, 30, 85 ).lighter( 120 ), 3.0 ) );
			// painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
			// painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
			// painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "EPUB" );
		// }

		// else {
			// painter.setPen( QPen( QColor( 156, 181, 142 ), 3.0 ) );
			// painter.drawRoundedRect( QRectF( 54, 92, 64, 27 ), 5.0, 5.0 );
			// painter.setFont( QFont( "CMU Sans Serif", 14, QFont::Black ) );
			// painter.drawText( QRectF( 54, 92, 64, 30 ), Qt::AlignCenter, "CBZ" );
		// }

		// painter.end();

		// fz_drop_device( mCtx, dev );
		// fz_drop_pixmap( mCtx, image );
		// fz_drop_page( mCtx, page );

		// if ( not thumb.save( hashPath, "png", 0 ) )
			// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
	// }

	// fz_always( mCtx ) {

		// fz_drop_document( mCtx, mFzDoc );
		// fz_drop_context( mCtx );
	// }

	// fz_catch( mCtx ) {
		// qDebug() << "Failed to create thumbnail:" << baseName( path ) << "Using default icon.";
		// return;
	// }
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( PdfPlugin, NBPdfPlugin );
#endif

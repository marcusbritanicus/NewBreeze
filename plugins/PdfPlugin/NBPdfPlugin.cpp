/*
	*
	* NBPdfPlugin.cpp - NewBreeze Video Thumbnailer Class
	*
*/

#include "NBPdfPlugin.hpp"
#include "NBPdfPeep.hpp"
#include <qdocumentview/PopplerDocument.hpp>

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
	}

	mimeList.removeDuplicates();

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBPdfPlugin::actionTrigger( Interface iface, QString fileType, QStringList nodes ) {
	/* @nodes[ 1 ] = file; @nodes[ 2 ] = hashPath */

	if ( iface == MimeTypeInterface ) {
		QStringList allowed = QStringList() << "PDF";
		if ( allowed.contains( fileType ) )
			makeThumbnail( nodes.at( 0 ), nodes.at( 1 ) );
	}

	else if( iface == NBPluginInterface::PreviewInterface ) {

		QString file = nodes.at( 0 );

		if ( not isFile( file ) )
			return;

		QMimeType mime = mimeDb.mimeTypeForFile( file );
		// if ( mime.name().contains( "pdf" ) or mime.name().contains( "epub" ) or mime.name().contains( "comicbook" ) ) {
		if ( mime.name().contains( "pdf" ) ) {
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
	doc->load();

	if ( doc->passwordNeeded() ) {
		qDebug() << "Encrypted document";
		return;
	}

	if ( doc->pageCount() ) {
		QImage pic = doc->renderPage( 0, doc->pageSize( 0 ).scaled( 128, 128, Qt::KeepAspectRatio ).toSize(), QDocumentRenderOptions() );

		QPixmap thumb( QSize( 128, 128 ) );
		thumb.fill( Qt::transparent );

		QPainter painter( &thumb );
		painter.drawImage( QRectF( QPointF( ( 128 - pic.width() ) / 2, ( 128 - pic.height() ) / 2 ), QSizeF( pic.size() ) ), pic );
		painter.end();

		thumb.save( hashPath, "PNG" );
	}
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( PdfPlugin, NBPdfPlugin );
#endif

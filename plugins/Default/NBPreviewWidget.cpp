/*
	*
	* NBPreviewWidget.cpp - NewBreeze PreviewWidget Class
	*
*/

#include "NBPreviewWidget.hpp"

/* Name of the plugin */
QString NBPreviewWidget::name() {

	return "MarkdownPreview";
};

/* The plugin version */
QString NBPreviewWidget::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBPreviewWidget::actions( QStringList nodes ) {

	QList<QAction*> actList;

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {
		QMimeType mime = mimeDb.mimeTypeForFile( nodes.at( 0 ) );

		QAction *act = new QAction( QIcon( ":/icons/emblem-unmounted.png" ), "&Peek", this );

		if ( mime == mimeDb.mimeTypeForFile( "file.odt" ) ) {
			NBOdfOgle *peek = new NBOdfOgle( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime == mimeDb.mimeTypeForFile( "file.pdf" ) ) {
			NBPdfPeep *peek = new NBPdfPeep( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime == mimeDb.mimeTypeForFile( "file.djvu" ) ) {
			NBDjvuDisplay *peek = new NBDjvuDisplay( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( ( mime == mimeDb.mimeTypeForFile( "file.html" ) ) or ( mime.name() == "text/html" ) ) {
			NBWebWatch *peek = new NBWebWatch( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime.name().contains( "image" ) ) {
			NBImagePeek *peek = new NBImagePeek( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime.name().contains( "text" ) or mime.allAncestors().filter( "text" ).count() ) {
			NBWordView *peek = new NBWordView( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		actList << act;
	}

	return actList;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interface NBPreviewWidget::interface() {

	return NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBPreviewWidget::type() {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBPreviewWidget::contexts() {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList NBPreviewWidget::mimetypes() {

	QStringList mimeList;
	/* DjVu Mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.djvu" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.djvu" ).aliases();

	/* Pdf mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.pdf" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.pdf" ).aliases();

	/* Html mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.html" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.html" ).aliases();

	/* ODT mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.odt" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.odt" ).aliases();

	/* Image mime types */
	Q_FOREACH( QByteArray arr, QImageReader::supportedImageFormats() ) {
		mimeList << mimeDb.mimeTypeForFile( QString( "file.%1" ).arg( QString( arr ) ) ).name();
		mimeList << mimeDb.mimeTypeForFile( QString( "file.%1" ).arg( QString( arr ) ) ).aliases();
	}

	/* Text mime type */
	Q_FOREACH( QMimeType mime, mimeDb.allMimeTypes() ) {
		if ( mime.name().contains( "text" ) or mime.allAncestors().filter( "text" ).count() ) {
			mimeList << mime.name();
		}
	}

	return mimeList;
};

void NBPreviewWidget::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( DefaultPeekPlugins, NBPreviewWidget );
#endif

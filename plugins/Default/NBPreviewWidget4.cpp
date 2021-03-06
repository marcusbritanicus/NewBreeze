/*
	*
	* NBPreviewWidget4.cpp - NewBreeze PreviewWidget Class
	*
*/

#include "NBPreviewWidget4.hpp"

/* Name of the plugin */
QString NBPreviewWidget::name() {

	return "DefaultPreview";
};

/* The plugin version */
QString NBPreviewWidget::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBPreviewWidget::actions( Interface, QStringList nodes ) {

	QList<QAction*> actList;

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {
		QString previewNode = nodes.at( 0 );
		QMimeType mime = mimeDb.mimeTypeForFile( nodes.at( 0 ) );

		QAction *act = new QAction( QIcon( ":/icons/peek.png" ), "&Peek", this );

		if ( mime == mimeDb.mimeTypeForFile( "file.odt" ) ) {
			NBOdfOgle *peek = new NBOdfOgle( previewNode );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime.name().contains( "djvu" ) ) {
			NBDjvuDisplay *peek = new NBDjvuDisplay( previewNode );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime.name().contains( "html" ) or mime.name().contains( "xml" ) ) {
			NBWebWatch *peek = new NBWebWatch( previewNode );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		else if ( mime.name().startsWith( "image" ) or ( mime.name() == "video/x-mng" ) ) {
			NBImagePeek *peek = new NBImagePeek( previewNode );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			connect( act, SIGNAL( triggered() ), peek, SLOT( exec() ) );
		}

		actList << act;
	}

	return actList;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBPreviewWidget::interfaces() {

	return Interfaces() << NBPluginInterface::PreviewInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBPreviewWidget::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBPreviewWidget::contexts( Interface ) {

	return Contexts() << NBPluginInterface::File;
};

/* Mimetypes handled by the plugin */
QStringList NBPreviewWidget::mimetypes() {

	QStringList mimeList;

	/* DjVu Mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.djvu" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.djvu" ).aliases();
	mimeList << "image/vnd.djvu+multipage";

	/* Html mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.html" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.html" ).aliases();
	mimeList << "application/xhtml+xml" << "text/html" << "applixation/xml";

	/* ODT mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.odt" ).name();
	mimeList << mimeDb.mimeTypeForFile( "file.odt" ).aliases();

	/* Image mime types */
	Q_FOREACH( QByteArray arr, QImageReader::supportedImageFormats() ) {
		mimeList << mimeDb.mimeTypeForFile( QString( "file.%1" ).arg( QString( arr ) ) ).name();
		mimeList << mimeDb.mimeTypeForFile( QString( "file.%1" ).arg( QString( arr ) ) ).aliases();
	}

	mimeList.removeDuplicates();
	mimeList.removeAll( "application/octet-stream" );

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBPreviewWidget::actionTrigger( Interface, QString, QStringList nodes ) {

	if ( ( nodes.count() == 1 ) and isFile( nodes.at( 0 ) ) ) {
		QMimeType mime = mimeDb.mimeTypeForFile( nodes.at( 0 ) );

		if ( mime == mimeDb.mimeTypeForFile( "file.odt" ) ) {
			NBOdfOgle *peek = new NBOdfOgle( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			peek->exec();
		}

		else if ( mime.name().contains( "djvu" ) ) {
			NBDjvuDisplay *peek = new NBDjvuDisplay( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			peek->exec();
		}

		else if ( mime.name().contains( "html" ) or mime.name().contains( "xml" ) ) {
			NBWebWatch *peek = new NBWebWatch( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			peek->exec();
		}

		else if ( mime.name().startsWith( "image" ) or ( mime.name() == "video/x-mng" ) ) {
			NBImagePeek *peek = new NBImagePeek( nodes.at( 0 ) );
			peek->setWindowFlags( peek->windowFlags() | Qt::FramelessWindowHint );
			peek->exec();
		}
	}
};

void NBPreviewWidget::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( DefaultPeekPlugins, NBPreviewWidget );
#endif

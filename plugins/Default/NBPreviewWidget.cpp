/*
	*
	* NBPreviewWidget.cpp - NewBreeze PreviewWidget Class
	*
*/

#include <NBPreviewWidget.hpp>

QStringList NBPreviewWidget::mimeTypesHandled() const {

	QStringList mimeList;
	/* DjVu Mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.djvu" ).name();

	/* Pdf mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.pdf" ).name();

	/* Html mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.html" ).name();

	/* ODT mime type */
	mimeList << mimeDb.mimeTypeForFile( "file.odt" ).name();

	/* Image mime types */
	Q_FOREACH( QByteArray arr, QImageReader::supportedImageFormats() )
		mimeList << mimeDb.mimeTypeForFile( QString( "file.%1" ).arg( QString( arr ) ) ).name();

	/* Text mime type */
	Q_FOREACH( QMimeType mime, mimeDb.allMimeTypes() )
		if ( mime.name().contains( "text" ) or mime.allAncestors().filter( "text" ).count() )
			mimeList << mime.name();

	return mimeList;
};

QDialog* NBPreviewWidget::getPreviewWidget( const QString &path ) {

	QMimeType mime = mimeDb.mimeTypeForFile( path );

	QStringList list = mimeTypesHandled();
	if ( not list.contains( mime.name() ) )
		return new QDialog();

	if ( mime == mimeDb.mimeTypeForFile( "file.odt" ) )
		return new NBOdfOgle( path );

	else if ( mime == mimeDb.mimeTypeForFile( "file.pdf" ) )
		return new NBPdfPeep( path );

	else if ( mime == mimeDb.mimeTypeForFile( "file.djvu" ) )
		return new NBDjvuDisplay( path );

	else if ( ( mime == mimeDb.mimeTypeForFile( "file.html" ) ) or ( mime.name() == "text/html" ) )
		return new NBWebWatch( path );

	else if ( mime.name().contains( "image" ) )
		return new NBImagePeek( path );

	else if ( mime.name().contains( "text" ) or mime.allAncestors().filter( "text" ).count() )
		return new NBWordView( path );

	else
		return new QDialog();
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( DefaultPeekPlugins, NBPreviewWidget );
#endif

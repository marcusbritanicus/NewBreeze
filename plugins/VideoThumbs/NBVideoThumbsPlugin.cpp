/*
	*
	* NBVideoThumbsPlugin.cpp - NewBreeze Video Thumbnailer Class
	*
*/

#include "NBVideoThumbsPlugin.hpp"

static QMimeDatabase mimeDb;

/* Name of the plugin */
QString NBVideoThumbsPlugin::name() {

	return "VideoThumbs";
};

/* The plugin version */
QString NBVideoThumbsPlugin::version() {

	return "3.0.0";
};

/* The QAction hooks for menus/toolbars */
QList<QAction*> NBVideoThumbsPlugin::actions( Interface, QStringList ) {

	return QList<QAction*>();
};

/* Interface type: preview, rename etc */
NBPluginInterface::Interfaces NBVideoThumbsPlugin::interfaces() {

	return Interfaces() << NBPluginInterface::MimeTypeInterface;
};

/* Interface type: preview, rename etc */
NBPluginInterface::Type NBVideoThumbsPlugin::type( Interface ) {

	return NBPluginInterface::Enhancement;
};

/* Plugin load contexts */
NBPluginInterface::Contexts NBVideoThumbsPlugin::contexts( Interface ) {

	return Contexts() << NBPluginInterface::None;
};

/* Mimetypes handled by the plugin */
QStringList NBVideoThumbsPlugin::mimetypes() {

	QStringList mimeList;
	Q_FOREACH( QMimeType type, mimeDb.allMimeTypes() ) {

		if ( type.name().startsWith( "video" ) )
			mimeList << type.name();
	}

	mimeList.removeDuplicates();
	mimeList.removeAll( "video/mng" );

	return mimeList;
};

/* Invoke slots called called by triggering the actions */
void NBVideoThumbsPlugin::actionTrigger( Interface, QString, QStringList nodes ) {

	ffmpegthumbnailer::VideoThumbnailer *mThumbnailer = new ffmpegthumbnailer::VideoThumbnailer();
	mThumbnailer->setSeekPercentage( 33 );

	ffmpegthumbnailer::FilmStripFilter mFilmStrip;
	mThumbnailer->addFilter( &mFilmStrip );

	QImage thumb;
	QString vFile = nodes.at( 0 );
	QString hashPath = nodes.at( 1 );

	try {
		std::vector<uint8_t> pixelBuffer;
		mThumbnailer->setThumbnailSize( 128 );
		mThumbnailer->generateThumbnail( vFile.toStdString(), Png, pixelBuffer );
		thumb.loadFromData( &pixelBuffer.front(), pixelBuffer.size(), "PNG" );
	}

	catch ( std::exception &e ) {
	}

	if ( thumb.isNull() or not thumb.save( hashPath, "PNG", 0 ) ) {

		QFile::copy( ":/icons/video.png", hashPath );
		qDebug() << "Failed to create thumbnail:" << baseName( vFile ) << "Using default icon.";
	}
};

void NBVideoThumbsPlugin::setCaller( QWidget *caller ) {

	mParent = caller;
};

#if QT_VERSION < 0x050000
	Q_EXPORT_PLUGIN2( VideoThumbs, NBVideoThumbsPlugin );
#endif

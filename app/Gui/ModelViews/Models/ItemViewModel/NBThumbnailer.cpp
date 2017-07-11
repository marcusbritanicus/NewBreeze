/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer
	*
*/

#include "NBThumbnailer.hpp"

static QList<QByteArray> supported = QImageReader::supportedImageFormats();

static inline int isImage( const struct dirent* entry ) {

	QByteArray suffix = QFileInfo( entry->d_name ).suffix().toLower().toLocal8Bit();
	if ( supported.contains( suffix ) )
		return true;

	return false;
};

static inline QStringList imageFiles( QString path ) {

	struct dirent **fileList;
	int entries = scandir( path.toLocal8Bit().data(), &fileList, isImage, NULL );
	if ( entries > 0 ) {
		QStringList files;
		for( int i = 0; i < entries; i++ ) {
			/* Ignore . and .. */
			struct dirent *entry = fileList[ i ];
			if ( not strcmp( entry->d_name, "." ) or not strcmp( entry->d_name, ".." ) )
				continue;

			files << path + QString::fromLocal8Bit( entry->d_name );
		}

		return files;
	}

	return QStringList();
};

void NBThumbnailer::createThumbnails( QString path ) {

	if ( isRunning() )
		terminate();

	mPath = QString( path );

	mTerminate = false;
	start();
};

void NBThumbnailer::run() {

	QStringList files = imageFiles( mPath );
	Q_FOREACH( QString file, files ) {

		if ( mTerminate )
			break;

		/* If @path is non-existent */
		if ( not exists( file ) )
			continue;

		/* Create a hash of the path */
		QString hashPath = thumbsDir + MD5( file );

		/* If the thumbnail is already formed */
		if ( exists( hashPath ) )
			continue;

		/* Cheat scaling: http://blog.qt.io/blog/2009/01/26/creating-thumbnail-preview/ */
		QImage thumb = QImage( file ).scaled( 512, 512, Qt::KeepAspectRatio ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );

		if ( thumb.save( hashPath, "PNG", 0 ) )
			emit updateNode( file );

		else
			qDebug() << "Failed to create thumbnail:" << baseName( file );
	}
};

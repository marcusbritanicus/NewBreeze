/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer
	*
*/

#include "NBThumbnailer.hpp"
#include "NBPluginInterface.hpp"

static QList<QByteArray> supported = QImageReader::supportedImageFormats();
static QStringList odfformat = QStringList() << "odt" << "odp" << "ods" << "odg";

static inline int isImage( const struct dirent* entry ) {

	QByteArray suffix = QFileInfo( entry->d_name ).suffix().toLower().toLocal8Bit();
	if ( supported.contains( suffix ) )
		return true;

	return false;
};

static inline int isODF( const struct dirent* entry ) {

	QByteArray suffix = QFileInfo( entry->d_name ).suffix().toLower().toLocal8Bit();
	if ( odfformat.contains( suffix ) )
		return true;

	return false;
};

static inline int isVideo( const struct dirent* entry ) {

	QMimeType mt = mimeDb.mimeTypeForFile( entry->d_name );
	return ( mt.name().startsWith( "video/" ) and mt.name().compare( "video/mng" ) );
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

static inline QStringList odffiles( QString path ) {

	struct dirent **fileList;
	int entries = scandir( path.toLocal8Bit().data(), &fileList, isODF, NULL );
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

static inline QStringList videoFiles( QString path ) {

	struct dirent **fileList;
	int entries = scandir( path.toLocal8Bit().data(), &fileList, isVideo, NULL );
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

	/* Image Files */

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

		if ( thumb.save( hashPath, "PNG", 0 ) ) {
			emit updateNode( file );
		}

		else {
			QFile::copy( ":/icons/image.png", hashPath );
			qDebug() << "Failed to create thumbnail:" << baseName( file ) << "Using default icon.";
		}
	}

	/* ODf Files */

	files = odffiles( mPath );
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

		/* Open as archive */
		NBArchive *odf = new NBArchive( file );
		odf->setDestination( "/tmp/NewBreeze_odf/" );
		if ( not odf->extractMember( "Thumbnails/thumbnail.png" ) ) {
			QFile::copy( "/tmp/NewBreeze_odf/Thumbnails/thumbnail.png", hashPath );
			system( "rm -rf /tmp/NewBreeze_odf/*" );
			emit updateNode( file );
		}
	}

	/* Video Files */

	QSettings nbpset( "NewBreeze", "Plugins" );
	QString pluginSo;

	Q_FOREACH( QString pth, nbpset.value( "PluginPaths" ).toStringList() ) {
		if ( exists( pth + "/libVideoThumbs.so" ) )
			pluginSo = QDir( pth ).absoluteFilePath( "libVideoThumbs.so" );
	}

	if ( pluginSo.count() ) {

		QPluginLoader loader( pluginSo );
		QObject *pObject = loader.instance();
		if ( pObject ) {
			NBPluginInterface *plugin = qobject_cast<NBPluginInterface*>( pObject );
			files = videoFiles( mPath );
			Q_FOREACH( QString file, files ) {
				/* If @path is non-existent */
				if ( not exists( file ) )
					continue;

				/* Create a hash of the path */
				QString hashPath = thumbsDir + MD5( file );

				/* If the thumbnail is already formed */
				if ( exists( hashPath ) )
					continue;

				plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, QString(), QStringList() << file << hashPath );
				emit updateNode( file );
			}
		}
	}
};

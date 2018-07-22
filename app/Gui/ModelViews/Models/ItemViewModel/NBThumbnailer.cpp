/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer
	*
*/

#include "NBThumbnailer.hpp"
#include "NBPluginInterface.hpp"

static QList<QByteArray> supported = QImageReader::supportedImageFormats();
static QStringList odfformat = QStringList() << "odt" << "odp" << "ods" << "odg";

void NBThumbnailer::createThumbnails( QStringList nodes ) {

	if ( isRunning() )
		terminate();

	images.clear();
	documents.clear();
	videos.clear();
	pdfs.clear();
	djvus.clear();

	Q_FOREACH( QString node, nodes ) {
		QString mime = mimeDb.mimeTypeForFile( node ).name();
		if ( mime.startsWith( "image" ) and not mime.contains( "djv" ) ) {
			if ( supported.contains( QFileInfo( node.toLower() ).suffix().toLocal8Bit() ) )
				images << node;
		}

		else if ( odfformat.contains( QFileInfo( node.toLower() ).suffix() ) ) {
			documents << node;
		}

		else if ( mime.startsWith( "video/" ) and mime.compare( "video/mng" ) ) {
			videos << node;
		}

		else if ( mime.contains( "pdf" ) ) {
			pdfs << node;
		}

		else if ( mime.contains( "djv" ) ) {
			djvus << node;
		}
	}

	mTerminate = false;
	start();
};

void NBThumbnailer::createThumbnails( QString path, QStringList nodes ) {

	if ( isRunning() )
		terminate();

	images.clear();
	documents.clear();
	videos.clear();
	pdfs.clear();
	djvus.clear();

	Q_FOREACH( QString node, nodes ) {
		QString mime = mimeDb.mimeTypeForFile( path + node ).name();
		if ( mime.startsWith( "image" ) and not mime.contains( "djv" ) ) {
			if ( supported.contains( QFileInfo( node.toLower() ).suffix().toLocal8Bit() ) )
				images << path + node;
		}

		else if ( odfformat.contains( QFileInfo( node.toLower() ).suffix() ) ) {
			documents << path + node;
		}

		else if ( mime.startsWith( "video/" ) and mime.compare( "video/mng" ) ) {
			videos << path + node;
		}

		else if ( mime.contains( "pdf" ) ) {
			pdfs << path + node;
		}

		else if ( mime.contains( "djv" ) ) {
			djvus << path + node;
		}
	}

	mTerminate = false;
	start();
};

void NBThumbnailer::run() {

	/* Image Files */
	if ( Settings->View.ImagePreview ) {
		Q_FOREACH( QString file, images ) {

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
				qDebug() << "Failed to create thumbnail:" << baseName( file ) << "Using default icon.";
			}

			qApp->processEvents();
		}
	}

	/* ODF Files */
	if ( Settings->View.OdfPreview ) {
		Q_FOREACH( QString file, documents ) {

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

			qApp->processEvents();
		}
	}

	/* Plugins based thumbnailers */
	QSettings nbpset( "NewBreeze", "Plugins" );
	QString pluginSo;

	Q_FOREACH( QString pth, nbpset.value( "PluginPaths" ).toStringList() ) {
		if ( exists( pth + "/libThumbnailer.so" ) )
			pluginSo = QDir( pth ).absoluteFilePath( "libThumbnailer.so" );
	}

	if ( pluginSo.count() ) {

		QPluginLoader loader( pluginSo );
		QObject *pObject = loader.instance();
		if ( pObject ) {
			NBPluginInterface *plugin = qobject_cast<NBPluginInterface*>( pObject );

			/* Video files */
			if ( Settings->View.VideoPreview ) {
				Q_FOREACH( QString file, videos ) {
					/* If @path is non-existent */
					if ( not exists( file ) )
						continue;

					/* Create a hash of the path */
					QString hashPath = thumbsDir + MD5( file );

					/* If the thumbnail is already formed */
					if ( exists( hashPath ) )
						continue;

					plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, "Video", QStringList() << file << hashPath );
					emit updateNode( file );

					qApp->processEvents();
				}
			}

			/* PDF Files */
			if ( Settings->View.PdfPreview ) {
				Q_FOREACH( QString file, pdfs ) {
					/* If @path is non-existent */
					if ( not exists( file ) )
						continue;

					/* Create a hash of the path */
					QString hashPath = thumbsDir + MD5( file );

					/* If the thumbnail is already formed */
					if ( exists( hashPath ) )
						continue;

					plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, "PDF", QStringList() << file << hashPath );
					emit updateNode( file );

					qApp->processEvents();
				}
			}

			/* DjVu files */
			if ( Settings->View.DjVuPreview ) {
				Q_FOREACH( QString file, djvus ) {
					/* If @path is non-existent */
					if ( not exists( file ) )
						continue;

					/* Create a hash of the path */
					QString hashPath = thumbsDir + MD5( file );

					/* If the thumbnail is already formed */
					if ( exists( hashPath ) )
						continue;

					plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, "DjVu", QStringList() << file << hashPath );
					emit updateNode( file );

					qApp->processEvents();
				}
			}
		}
	}
};

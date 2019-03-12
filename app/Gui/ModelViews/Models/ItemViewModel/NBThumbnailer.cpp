/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer
	*
*/

#include <libexif/exif-loader.h>

#include "NBThumbnailer.hpp"
#include "NBPluginInterface.hpp"

#include "Epeg.h"

static QList<QByteArray> supported = QImageReader::supportedImageFormats();
static QStringList odfformat = QStringList() << "odt" << "odp" << "ods" << "odg";

inline static bool saveExifThumbnail( QString path, QString hashPath ) {

	ExifLoader* exif_loader = exif_loader_new();

	QFile jpeg( path );
	if ( jpeg.open( QFile::ReadOnly ) ) {
		while ( true ) {
			QByteArray exif = jpeg.read( 4096 );
			if ( exif_loader_write( exif_loader, ( unsigned char* )exif.data(), exif.size() ) == 0 )
				break;
		}

		ExifData* exif_data = exif_loader_get_data( exif_loader );
		exif_loader_unref( exif_loader );

		if ( exif_data ) {
			// If an embedded thumbnail is available, load it
			if ( exif_data->data ) {
				QImage thumbnail;
				thumbnail.loadFromData( exif_data->data, exif_data->size );
				exif_data_unref( exif_data );
				return thumbnail.save( hashPath, "JPG" );
			}

			else {

				exif_data_unref( exif_data );
				return false;
			}

			return true;
		}

		return false;
	}

	return false;
};

void NBThumbnailer::createThumbnails( QStringList nodes ) {

	/* If its not running, clear previous data */
	if ( not isRunning() ) {
		images.clear();
		documents.clear();
		videos.clear();
		pdfs.clear();
		djvus.clear();
		epubs.clear();
		cbzs.clear();
	}

	/* If running, terminate; don't clear nodes */
	else {
		terminate();
	}

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

		else if ( mime.contains( "epub" ) ) {
			epubs << node;
		}

		else if ( mime.contains( "comicbook" ) ) {
			cbzs << node;
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
	epubs.clear();
	cbzs.clear();

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

		else if ( mime.contains( "epub" ) ) {
			epubs << path + node;
		}

		else if ( mime.contains( "comicbook" ) ) {
			epubs << path + node;
		}
	}

	mTerminate = false;
	start();
};

void NBThumbnailer::run() {

	/* Image Files */
	if ( Settings->value( "View/ImagePreview" ) ) {
		Q_FOREACH( QString file, images ) {

			if ( mTerminate )
				return;

			/* If @path is non-existent */
			if ( not exists( file ) )
				continue;

			/* Do not generate thumbnails for files in the thumbnail directory */
			if ( dirName( file ) == thumbsDir )
				continue;

			/* Create a hash of the path */
			QString hashPath = thumbsDir + MD5( file );

			/* If the thumbnail is already formed */
			if ( exists( hashPath ) )
				continue;

			// if ( saveJpegThumb( file.toLocal8Bit().constData(), hashPath.toLocal8Bit().constData() ) ) {

				// emit updateNode( file );
			// }

			/* Do we need to save them? */
			if ( saveExifThumbnail( file, hashPath ) ) {

				emit updateNode( file );
			}

			else {
				qDebug() << "Using inbuilt scaling:" << baseName( file );
				/* Cheat scaling: http://blog.qt.io/blog/2009/01/26/creating-thumbnail-preview/ */
				QImage thumb = QImage( file ).scaled( 512, 512, Qt::KeepAspectRatio, Qt::FastTransformation ).scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );

				if ( file.endsWith( "jpg" ) or file.endsWith( "jpeg" ) ) {
					if ( thumb.save( hashPath, "JPG" ) )
						emit updateNode( file );

					else
						qDebug() << "Failed to create thumbnail:" << baseName( file ) << "Using default icon.";
				}

				else {
					if ( thumb.save( hashPath, "PNG", 0 ) )
						emit updateNode( file );

					else
						qDebug() << "Failed to create thumbnail:" << baseName( file ) << "Using default icon.";
				}
			}
		}
	}

	/* ODF Files */
	if ( Settings->value( "View/OdfPreview" ) ) {
		Q_FOREACH( QString file, documents ) {

			if ( mTerminate )
				return;

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
			odf->extractMember( "Thumbnails/thumbnail.png" );
			if ( not odf->exitStatus() ) {
				QFile::copy( "/tmp/NewBreeze_odf/Thumbnails/thumbnail.png", hashPath );
				system( "rm -rf /tmp/NewBreeze_odf/*" );
				emit updateNode( file );
			}
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
			if ( Settings->value( "View/VideoPreview" ) ) {
				Q_FOREACH( QString file, videos ) {
					if ( mTerminate )
						return;

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
				}
			}

			/* DjVu files */
			if ( Settings->value( "View/DjVuPreview" ) ) {
				Q_FOREACH( QString file, djvus ) {
					if ( mTerminate )
						return;

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
				}
			}

			loader.unload();
		}
	}

	Q_FOREACH( QString pth, nbpset.value( "PluginPaths" ).toStringList() ) {
		if ( exists( pth + "/libPdfPlugin.so" ) )
			pluginSo = QDir( pth ).absoluteFilePath( "libPdfPlugin.so" );
	}

	if ( pluginSo.count() ) {

		QPluginLoader loader( pluginSo );
		QObject *pObject = loader.instance();
		if ( pObject ) {
			NBPluginInterface *plugin = qobject_cast<NBPluginInterface*>( pObject );

			/* PDF Files */
			if ( Settings->value( "View/PdfPreview" ) ) {
				Q_FOREACH( QString file, pdfs ) {
					if ( mTerminate )
						return;

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
				}
			}

			/* ePub files */
			if ( Settings->value( "View/ePubPreview" ) ) {
				Q_FOREACH( QString file, epubs ) {
					if ( mTerminate )
						return;

					/* If @path is non-existent */
					if ( not exists( file ) )
						continue;

					/* Create a hash of the path */
					QString hashPath = thumbsDir + MD5( file );

					/* If the thumbnail is already formed */
					if ( exists( hashPath ) )
						continue;

					plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, "ePub", QStringList() << file << hashPath );
					emit updateNode( file );
				}
			}

			/* cbz files */
			if ( Settings->value( "View/ePubPreview" ) ) {
				Q_FOREACH( QString file, cbzs ) {
					if ( mTerminate )
						return;

					/* If @path is non-existent */
					if ( not exists( file ) )
						continue;

					/* Create a hash of the path */
					QString hashPath = thumbsDir + MD5( file );

					/* If the thumbnail is already formed */
					if ( exists( hashPath ) )
						continue;

					plugin->actionTrigger( NBPluginInterface::MimeTypeInterface, "CBZ", QStringList() << file << hashPath );
					emit updateNode( file );
				}
			}

			loader.unload();
		}
	}
};

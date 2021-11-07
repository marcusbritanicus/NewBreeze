/*
	*
	* NBThumbnailer.hpp - NewBreeze Thumbnailer
	*
*/

#include "NBThumbnailer.hpp"
#include "NBItemViewModel.hpp"
#include "NBPluginInterface.hpp"

#include <libarchiveqt.h>

static QList<QByteArray> supported = QImageReader::supportedImageFormats();
static QStringList odfformat = QStringList() << "odt" << "odp" << "ods" << "odg";

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

			/* Cheat scaling: http://blog.qt.io/blog/2009/01/26/creating-thumbnail-preview/ */
			QImageReader picReader( file );

			QImage pic = picReader.read();
			if ( pic.isNull() )
				continue;

			pic = pic.scaled( 512, 512, Qt::KeepAspectRatio, Qt::FastTransformation );
			pic = pic.scaled( 128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation );

			QPixmap thumb( QSize( 128, 128 ) );
			thumb.fill( Qt::transparent );

			QPainter painter( &thumb );
			painter.drawImage( QRectF( QPointF( ( 128 - pic.width() ) / 2, ( 128 - pic.height() ) / 2 ), QSizeF( pic.size() ) ), pic );
			painter.end();

			NBItemViewModel::iconMap.insert( file, thumb );

			emit updateNode( file );
			emit saveThumb( file );
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
			LibArchiveQt *odf = new LibArchiveQt( file );
			odf->setDestination( "/tmp/NewBreeze_odf/" );
			odf->extractMember( "Thumbnails/thumbnail.png" );
			if ( not odf->exitStatus() ) {
				QFile::copy( "/tmp/NewBreeze_odf/Thumbnails/thumbnail.png", hashPath );
				removeDir( "/tmp/NewBreeze_odf" );
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

/* ##### NBThumbSaver Class ========================================================================================================== */
NBThumbSaver::NBThumbSaver() : QThread() {

	fileList.clear();
	isActive = false;
};

void NBThumbSaver::save( QString filename ) {

	if ( not fileList.contains( filename ) )
		fileList << filename;

	if ( not isActive )
		start();
};

void NBThumbSaver::run() {

	while ( true ) {
		if ( fileList.count() ) {
			isActive = true;

			QString file = fileList.takeFirst();
			QIcon icon = NBItemViewModel::iconMap.value( file );

			if ( not icon.isNull() ) {
				QString thumbmailPath = thumbsDir + MD5( file );
				icon.pixmap( 128 ).save( thumbmailPath, "PNG" );
			}
		}

		else {
			isActive = false;
			return;
		}
	}
};

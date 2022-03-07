/*
	*
	* NBFileInfoGatherer.cpp - NewBreeze FileInfoGatherer Class
	*
*/

#include "NBFileInfoGatherer.hpp"

NBQuickFileInfoGatherer::NBQuickFileInfoGatherer() {
};

QString NBQuickFileInfoGatherer::permStr( mode_t fileMode ) {
	QString permStr;

	permStr += ( ( fileMode & S_IRUSR ) ? "r" : "-" );
	permStr += ( ( fileMode & S_IWUSR ) ? "w" : "-" );
	permStr += ( ( fileMode & S_IXUSR ) ? "x" : "-" );

	permStr += ( ( fileMode & S_IRGRP ) ? "r" : "-" );
	permStr += ( ( fileMode & S_IWGRP ) ? "w" : "-" );
	permStr += ( ( fileMode & S_IXGRP ) ? "x" : "-" );

	permStr += ( ( fileMode & S_IROTH ) ? "r" : "-" );
	permStr += ( ( fileMode & S_IWOTH ) ? "w" : "-" );
	permStr += ( ( fileMode & S_IXOTH ) ? "x" : "-" );

	return permStr;
};

QString NBQuickFileInfoGatherer::formatSize( qint64 num ) {

	QString total;
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;

	if ( num >= tb ) total = QString( "%1 TiB" ).arg( QString::number( qreal( num ) / tb, 'f', 3 ) );
	else if ( num >= gb ) total = QString( "%1 GiB" ).arg( QString::number( qreal( num ) / gb, 'f', 2 ) );
	else if ( num >= mb ) total = QString( "%1 MiB" ).arg( QString::number( qreal( num ) / mb, 'f', 1 ) );
	else if ( num >= kb ) total = QString( "%1 KiB" ).arg( QString::number( qreal( num ) / kb,'f',1 ) );
	else total = QString( "%1 byte%2" ).arg( num ).arg( num > 1 ? "s": "" );

	return total;
}

QVariantList NBQuickFileInfoGatherer::getQuickFileInfo( QString path ) {

	/*
		*
		* [
		* 	dir/file/system
		*   rawsize
		*   icon ( string used with QIcon::fromTheme )
		*   name = 0
		*   size = 1
		*   type = 2
		*   mime = 3
		*   time = 4
		*   perm = 5
		*   ownr = 6
		*   mtpt = 7
		* ]
		*
	*/

	QVariantList info;

	struct stat statbuf;
	if ( stat( path.toLocal8Bit().data(), &statbuf ) != 0 )
		return QVariantList() << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";

	struct passwd *pwdinfo = getpwuid( statbuf.st_uid );

	if ( path == "/" ) {
		info << "dir";
		info << nChildren( "/" );
		info << "folder";
		info << "FileSystem";
		info << QString( "%1 item%2" ).arg( info.at( 1 ).toLongLong() ).arg( info.at( 1 ).toLongLong() > 1 ? "s": "" );
		info << "Directory";
		info << "inode/directory";
		info << QDateTime::fromTime_t( statbuf.st_mtime );
		info << permStr( statbuf.st_mode );
		info << "root";
		info << "/";

		return info;
	};

	// Start with raw node size
	// In case of directories size will be number of items

	// Add the node name
	QString name = path.split( "/", Qt::SkipEmptyParts ).takeLast();

	// Here insert the node type, formatted type, mime
	if ( S_ISDIR( statbuf.st_mode ) ) {
		info << "dir";
		info << nChildren( path );
		info << "folder";
		info << ( name.isEmpty() ? "FileSystem" : name );
		info << QString( "%1 item%2" ).arg( info.at( 1 ).toLongLong() ).arg( info.at( 1 ).toLongLong() > 1 ? "s": "" );
		info << "Directory";
		info << "inode/directory";
	}

	else if ( S_ISREG( statbuf.st_mode ) ) {
		// QMimeType mType = mimeDb.mimeTypeForFile( path );
		QStringList ext = name.split( ".", Qt::SkipEmptyParts );
		QStringList mimeData;

		/* We have only the file name, use QMimeDatabase */
		if ( ext.count() == 1 ) {
			QMimeType mType = mimeDb.mimeTypeForFile( path );
			mimeData = QStringList( { mType.comment(), mType.name(), mType.iconName() } );
		}

		/* Our file has an extension */
		else {
			if ( ext.count() >= 2 ) {
				/* Simple extension: .png, .txt, .jpg, etc */
				QString ext1 = ext.takeLast();
				/* More interesting extenstions: .tar.gz, .tar.xz etc */
				QString ext2 = ext.takeLast();

				if ( fileInfoHash.contains( ext1 + "." + ext2 ) )
					mimeData = fileInfoHash.value( ext1 + "." + ext2 );

				else
					mimeData = fileInfoHash.value( ext1 );
			}
		}

		if ( not mimeData.count() ) {
			qDebug() << "Unknown File Type:" << path;
			QMimeType mType = mimeDb.mimeTypeForFile( path );
			mimeData = QStringList( { mType.comment(), mType.name(), mType.iconName() } );
		}

		info << "file";
		info << quint64( statbuf.st_size );
		info << mimeData.at( 2 );
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << mimeData.at( 0 );
		info << mimeData.at( 1 );
	}

	else if ( S_ISCHR( statbuf.st_mode ) ) {
		info << "system";
		info << quint64( statbuf.st_size );
		info << "utilities-terminal";
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << "Character Device";
		info << "inode/chardevice";
	}

	else if ( S_ISBLK( statbuf.st_mode ) ) {
		info << "system";
		info << quint64( statbuf.st_size );
		info << "drive-harddisk";
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << "Block Device";
		info << "inode/blockdevice";
	}

	else if ( S_ISFIFO( statbuf.st_mode ) ) {
		info << "system";
		info << quint64( statbuf.st_size );
		info << "inode-fifo";
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << "FIFO Device";
		info << "inode/fifo";
	}

	else if ( S_ISSOCK( statbuf.st_mode ) ) {
		info << "system";
		info << quint64( statbuf.st_size );
		info << "inode-socket";
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << "Socket";
		info << "inode/socket";
	}

	else if ( S_ISLNK( statbuf.st_mode ) ) {
		// This is redundant mechanism
		info << "system";
		info << quint64( statbuf.st_size );
		info << "unknown";
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << "Link";
		info << "inode/symlink";
	}

	else
		return QVariantList() << "" << "" << "" << "" << "" << "" << "" << "" << "" << "";

	info << QDateTime::fromTime_t( statbuf.st_mtime );
	info << permStr( statbuf.st_mode );
	info << ( ( pwdinfo == NULL ) ? QString::number( statbuf.st_uid ) : pwdinfo->pw_name );
	info << path;

	return info;
};

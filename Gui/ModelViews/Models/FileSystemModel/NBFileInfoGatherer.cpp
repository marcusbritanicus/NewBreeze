/*
	*
	* NBFileInfoGatherer.cpp - NewBreeze FileInfoGatherer Class
	*
*/

#include <NBIconProvider.hpp>
#include <NBFileInfoGatherer.hpp>
#include <pwd.h>

NBQuickFileInfoGatherer::NBQuickFileInfoGatherer() {

	fileTypes = new QSettings( ":/Data/NBFSExtData.conf", QSettings::NativeFormat );
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
		* ]
		*
	*/

	QVariantList info;

	struct stat statbuf;
	if ( stat( qPrintable( path ), &statbuf ) != 0 )
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
		info << QDateTime::fromTime_t( statbuf.st_mtime ).toString( "ddd, dd MMM, yyyy" );
		info << permStr( statbuf.st_mode );
		info << "root";

		return info;
	};

	// Start with raw node size
	// In case of directories size will be number of items

	// Add the node name
	QString name = path.split( "/", QString::SkipEmptyParts ).takeLast();

	// Here insert the node type, formatted type, mime
	if ( S_ISDIR( statbuf.st_mode ) ) {
		info << "dir";
		info << nChildren( path );
		info << "folder";
		info << ( name.isEmpty() ? "/" : name );
		info << QString( "%1 item%2" ).arg( info.at( 1 ).toLongLong() ).arg( info.at( 1 ).toLongLong() > 1 ? "s": "" );
		info << "Directory";
		info << "inode/directory";
	}

	else if ( S_ISREG( statbuf.st_mode ) ) {
		QStringList baseExtInfo;
		if ( statbuf.st_mode & S_IXUSR )
			baseExtInfo = QStringList() << "Executable" << "application/octet-stream" << "application-octet-stream";
		else
			baseExtInfo = QStringList() << "Regular File" << "application/octet-stream" << "application-octet-stream";
		QStringList extInfo = fileTypes->value( name.section( ".", 1 ), baseExtInfo ).toStringList();

		info << "file";
		info << quint64( statbuf.st_size );
		info << extInfo.value( 2 );
		info << ( name.isEmpty() ? "/" : name );
		info << formatSize( statbuf.st_size );
		info << extInfo.value( 0 );
		info << extInfo.value( 1 );
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

	info << QDateTime::fromTime_t( statbuf.st_mtime ).toString( "ddd, dd MMM, yyyy" );
	info << permStr( statbuf.st_mode );
	info << ( ( pwdinfo == NULL ) ? QString::number( statbuf.st_uid ) : pwdinfo->pw_name );

	return info;
};

NBFileInfoGatherer::NBFileInfoGatherer() : QThread() {

	entryList.clear();
	rootPath.clear();
};

NBFileInfoGatherer::~NBFileInfoGatherer() {

	wait();
};

void NBFileInfoGatherer::gatherInfo( QStringList entries, QString root ) {

	entryList << entries;
	rootPath = QString( root );

	start();
};

void NBFileInfoGatherer::run() {

	foreach( QString entry, entryList ) {
		if ( Settings->Special.ClosingDown )
			break;

		QMimeType mimeType = mimeDb.mimeTypeForFile( rootPath + entry );
		QStringList info;

		info << NBIconProvider::icon( rootPath + entry, mimeType );
		info << mimeType.comment();
		info << mimeType.name();

		emit done( rootPath, entry, info );
	}
};

/*
	*
	* WTools.cpp - Some tools for TextEditor
	*
*/

#include <Global.hpp>
#include <Tools.hpp>

#include <sys/stat.h>
#include <sys/vfs.h>

#include <string>
#include <cstdio>

#include <magic.h>

QString dirName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( qPrintable( path ) );
	QString dirPth = QString( dirname( dupPath ) ) + "/";
	free( dupPath );

	return dirPth;
};

QString baseName( QString path ) {

	if ( path.endsWith( "/" ) )
		path.chop( 1 );

	char *dupPath = strdup( qPrintable( path ) );
	QString basePth = QString( basename( dupPath ) );
	free( dupPath );

	return basePth;
};

QString getMimeType( QString path ) {

	return mimeDb.mimeTypeForFile( path ).name();
};

QPixmap getPixmap( QString path ) {

	QString mimeIconName = mimeDb.mimeTypeForFile( path ).iconName();
	QString genericMimeIconName = mimeDb.mimeTypeForFile( path ).genericIconName();

	QIcon icon = QIcon::fromTheme( mimeIconName, QIcon::fromTheme( genericMimeIconName ) );

	if ( icon.isNull() )
		return QPixmap( ":/icons/text.png" );

	return icon.pixmap( 128, 128 );
};

qint64 getSize( QString path ) {

	QFileInfo info( path );
	return info.size();
};

mode_t getMode( QString path ) {

	struct stat fileAtts;
	if ( stat( path.toStdString().c_str(), &fileAtts ) != 0 ) {
		return -1;
	}

	return fileAtts.st_mode;
};

QString formatSize( qint64 num ) {

	QString total;
	const qint64 kb = 1024;
	const qint64 mb = 1024 * kb;
	const qint64 gb = 1024 * mb;
	const qint64 tb = 1024 * gb;

	if ( num >= tb ) total = QString( "%1 TiB" ).arg( QString::number( qreal( num ) / tb, 'f', 3 ) );
	else if ( num >= gb ) total = QString( "%1 GiB" ).arg( QString::number( qreal( num ) / gb, 'f', 2 ) );
	else if ( num >= mb ) total = QString( "%1 MiB" ).arg( QString::number( qreal( num ) / mb, 'f', 1 ) );
	else if ( num >= kb ) total = QString( "%1 KiB" ).arg( QString::number( qreal( num ) / kb,'f',1 ) );
	else total = QString( "%1 bytes" ).arg( num );

	return total;
};

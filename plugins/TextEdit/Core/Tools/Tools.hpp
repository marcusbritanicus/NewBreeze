/*
	*
	* WTools.hpp - WTools.cpp header
	*
*/

#pragma once

#include <QtGui>

/* Qt4 Equivalent of linux dirname and basename */
QString dirName( QString path );
QString baseName( QString path );

/* MimeType from QMimeDatabase */
QString getMimeType( QString path );
QPixmap getPixmap( QString path );

qint64 getSize( QString path );
mode_t getMode( QString path );

QString formatSize( qint64 );

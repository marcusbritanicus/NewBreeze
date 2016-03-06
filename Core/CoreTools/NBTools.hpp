/*
	*
	* NBTools.hpp - NBTools.cpp header
	*
*/

#pragma once
#ifndef NBTOOLS_HPP
#define NBTOOLS_HPP

#include <Global.hpp>

namespace DbgMsgPart {
	enum MsgPart {
		HEAD,
		BODY,
		TAIL,
		ONESHOT
	};
};

/* Qt4 Equivalent of linux dirname and basename */
QString dirName( QString path );
QString baseName( QString path );

/* MimeType from QMimeDatabase and libmagic */
QString getMimeType( QString path );
QString getMimeTypeAlt( QString path );

/* Format a string so that it can be used in bash as an argument */
QString termFormatString( QString file );

/* MD5 Cryptographic hash */
QString MD5( QString data );

/* Node recognition tools */
bool isFile( QString path );
bool isDir( QString path );
bool isLink( QString path );
bool exists( QString path );

/* Qt4 version of linux readlink(...) */
QString readLink( QString path );

/* Create a directory: mkdir -p */
/* http://fossies.org/linux/inadyn/libite/makepath.c */
int mkpath( QString, mode_t mode = 0755 );

/* Remove a directory and its contents */
bool removeDir( QString );

/* Is the path readable */
bool isReadable( QString path );
bool isWritable( QString path );

/* Get the number of children of a dir */
qint64 nChildren( QString );

/* Get the size of a directory and file */
qint64 getSize( QString path );

/* Mode of a file/folder */
mode_t getMode( QString path );

/* Get all the files and subdirs in directory */
QStringList recDirWalk( QString );

bool isText( QString path );
bool isExec( QString path );
bool isImage( QString path );
bool isArchive( QString path );
bool isArchiveAlt( QString mimeType );

/* Convert the numberic size to human readable string */
QString formatSize( qint64 );

/*  */
QStringList getTerminal();

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#endif

#if QT_VERSION >= 0x050000
	void NBMessageOutput5( QtMsgType, const QMessageLogContext&, const QString& );
#else
	void NBMessageOutput( QtMsgType, const char* );
#endif

void NBDebugMsg( DbgMsgPart::MsgPart, const char*, ... );

#endif

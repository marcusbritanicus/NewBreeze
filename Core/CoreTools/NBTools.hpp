/*
	*
	* NBTools.hpp - NBTools.cpp header
	*
*/

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

/* QDir with spe */
QDir NBDir( QString path );

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
QString readLink( QString path );
bool removeDir( QString );

/* Is the path readable */
bool isReadable( QString path );
bool isWritable( QString path );

/* Get the number of children of a dir and size of a file */
qint64 nChildren( QString );
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
QString getStyleSheet( QString, QString Style = "LightGray" );

/*  */
QStringList getTerminal();

void NBMessageOutput( QtMsgType, const char* );
void NBDebugMsg( DbgMsgPart::MsgPart, const char*, ... );

#endif

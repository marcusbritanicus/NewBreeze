/*
	*
	* NBTools.hpp - NBTools.cpp header
	*
*/

#pragma once

#include <Global.hpp>

/* Qt4 Equivalent of linux dirname and basename */
QString dirName( QString path );
QString baseName( QString path );

/* Node recognition tools */
bool isFile( QString path );
bool isDir( QString path );
bool isLink( QString path );
bool exists( QString path );

/* Create a directory: mkdir -p */
/* http://fossies.org/linux/inadyn/libite/makepath.c */
int mkpath( QString, mode_t mode = 0755 );

/* Is the path readable */
bool isReadable( QString path );
bool isWritable( QString path );

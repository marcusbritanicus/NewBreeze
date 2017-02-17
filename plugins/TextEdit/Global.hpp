/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include "newbreeze.hpp"

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
	#include <QtPrintSupport>
#endif
static QSettings settings( "TextEditor", "TextEditor" );
static QMimeDatabase mimeDb;

/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <iostream>
#include <cstring>
#include <libgen.h>

#include <QtGui>
#include <QtCore>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
	#include <QtPrintSupport>
	#include <QMimeDatabase>
	#include <QMimeType>
	#include <QStandardPaths>
#else
	#include <NBMimeDatabase.hpp>
	#include <NBMimeType.hpp>
	#include <NBStandardPaths.hpp>
#endif

static QSettings settings( "TextEditor", "TextEditor" );
static QMimeDatabase mimeDb;

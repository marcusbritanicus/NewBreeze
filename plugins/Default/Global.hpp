/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include <QtCore>
#include <QtWebKit>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
	#include <QMimeDatabase>
	#include <QMimeType>
	#include <QtWebKitWidgets>
#else
	#include <NBMimeDatabase.hpp>
	#include <NBMimeType.hpp>
#endif

static QMimeDatabase mimeDb;

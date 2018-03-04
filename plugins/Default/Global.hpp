/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include <QtCore>

#if QT_VERSION > 0x050000 && QT_VERSION < 0x050500
	#include <QtWidgets>
	#include <QtWebKit>
	#include <QtWebKitWidgets>
#elif QT_VERSION >= 0x050500
	#include <QtWidgets>
	#include <QtWebEngine>
	#include <QtWebEngineWidgets>
#else
	#include <QtWebKit>
#endif

#include "newbreeze.hpp"

static QMimeDatabase mimeDb;

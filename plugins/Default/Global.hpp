/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include <QtCore>

#if QT_VERSION < 0x050000
	#include <QtWebKit>
#els#if QT_VERSION < 0x050500
	#include <QtWidgets>
	#include <QtWebKitWidgets>
#else
	#include <QtWidgets>
	#include <QtWebEngine>
	#include <QtWebEngineWidgets>
#endif

#include "newbreeze.hpp"

static QMimeDatabase mimeDb;

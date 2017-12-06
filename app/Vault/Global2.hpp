/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtCore>
#include <QtGui>

#if QT_VERSION >= 0x050000
	#include <QtWidgets>
#else
	#include "QMimeDatabase.hpp"
#endif

#include "newbreeze.hpp"

static QMimeDatabase mimeDb;

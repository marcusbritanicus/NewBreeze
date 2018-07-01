/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

#include <QtGui>
#include <QtCore>

#include <unistd.h>
#include <libffmpegthumbnailer/videothumbnailer.h>
#include <libffmpegthumbnailer/filmstripfilter.h>
#include <libdjvu/ddjvuapi.h>

#if QT_VERSION >= 0x050000
	#include <poppler-qt5.h>
#else
	#include <poppler-qt4.h>
#endif


#include "newbreeze.hpp"

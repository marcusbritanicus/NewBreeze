/*
    *
    * Global.hpp - Globally used header
    *
*/

#pragma once

// Qt Headers
#include <QtGui>
#include "newbreeze.hpp"

#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>

#if QT_VERSION >= 0x050000
    #include <QCryptographicHash>
#else
    #include <QCryptographicHash5.hpp>
#endif

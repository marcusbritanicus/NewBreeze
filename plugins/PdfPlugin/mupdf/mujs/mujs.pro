TEMPLATE = lib
TARGET = mupdfthird

VERSION = 1.13.0

INCLUDEPATH += .

QT -= gui core gl

QMAKE_CFLAGS += -ffunction-sections -fdata-sections -pipe -O2 -DNDEBUG -fomit-frame-pointer -fPIC -I../include
CONFIG += warn_off silent static
CONFIG -= shared

# Headers
HEADERS += astnames.h
HEADERS += jsbuiltin.h
HEADERS += jscompile.h
HEADERS += jsi.h
HEADERS += jslex.h
HEADERS += jsparse.h
HEADERS += jsrun.h
HEADERS += jsvalue.h
HEADERS += mujs.h
HEADERS += opnames.h
HEADERS += regexp.h
HEADERS += utf.h

# Sources
# This one file encapsulates everything that we need
# to make a shared object/static library
SOURCES += one.c

OBJECTS_DIR = ../build/mujs/

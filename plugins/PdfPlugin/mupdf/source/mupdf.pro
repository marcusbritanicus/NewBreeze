TEMPLATE = lib
TARGET = mupdf

VERSION = 1.13.0

INCLUDEPATH += . ../include/ cbz fitz gprf html pdf svg xps ../mujs /usr/include/freetype2 /usr/include/harfbuzz /usr/include/openjpeg-2.3/
DEPENDPATH += . ../include/ cbz fitz gprf html pdf svg xps ../mujs

QT -= gui core gl

QMAKE_CFLAGS += -ffunction-sections -fdata-sections -pipe -O2 -fomit-frame-pointer -fPIC -I../include
DEFINES += NDEBUG JBIG_NO_MEMENTO TOFU NOCJK SHARE_JPEG NO_ICC HAVE_LIBCRYPTO

CONFIG += warn_off static
CONFIG -= shared

HEADERS += ../include/mupdf/*.h ../include/mupdf/fitz/*.h ../include/mupdf/helpers/*.h ../include/mupdf/pdf/*.h

# CBZ Sources
SOURCES += cbz/*.c

# Fitz Sources
SOURCES += fitz/*.c

# Gprf Sources
SOURCES += gprf/*.c

# Html Sources
SOURCES += html/*.c

# Pdf Sources
SOURCES += pdf/*.c

# Svg Sources
SOURCES += svg/*.c

# Xps Sources
SOURCES += xps/*.c

# Generated Sources
SOURCES += generated/*.c

OBJECTS_DIR = ../build/source/

CONFIG += warn_off silent

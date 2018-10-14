TEMPLATE = lib
TARGET = PdfPlugin4

# Same as NewBreeze version
VERSION = "3.0.0"

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# Common Locations
# ================
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

# MuPDF locations
# ===============
INCLUDEPATH += mupdf/source/cbz mupdf/source/fitz mupdf/source/gprf mupdf/source/html mupdf/source/pdf mupdf/source/svg mupdf/source/xps mupdf/mujs
INCLUDEPATH += /usr/include/freetype2 /usr/include/harfbuzz
DEPENDPATH += mupdf/source/cbz mupdf/source/fitz mupdf/source/gprf mupdf/source/html mupdf/source/pdf mupdf/source/svg mupdf/source/xps mupdf/mujs

# MuPDF and PdfPeep
# =================
INCLUDEPATH += mupdf/include/ PdfPeep
DEPENDPATH += mupdf/include/ PdfPeep

# cflags and defines
# ==================
QMAKE_CFLAGS += -ffunction-sections -fdata-sections -pipe -O2 -fomit-frame-pointer -fPIC -fno-dce -fno-dse -fno-tree-dce -fno-tree-dse
QMAKE_CFLAGS += $$system( pkg-config --cflags libopenjp2 )					# openJP2
QMAKE_CPPFLAGS += -fPIC -fno-dce -fno-dse -fno-tree-dce -fno-tree-dse
DEFINES += NDEBUG JBIG_NO_MEMENTO TOFU NOCJK SHARE_JPEG NO_ICC HAVE_LIBCRYPTO

# LIBS
# ====
LIBS         += -L/usr/lib -lharfbuzz -lfreetype -lz -ljpeg -lopenjp2 -ljbig2dec

# Poppler
# =======
equals( QT_MAJOR_VERSION, 4 ) {
	INCLUDEPATH  += /usr/include/poppler/qt4
	LIBS += -lpoppler-qt4
}

equals( QT_MAJOR_VERSION, 5 ) {
	INCLUDEPATH  += /usr/include/poppler/qt5
	LIBS += -lpoppler-qt5
}

# Silent and No warning
# =====================
CONFIG += warn_off silent

# Plugin Mode
# ===========
CONFIG += plugin

# MuJS
# ====
HEADERS += mupdf/mujs/astnames.h
HEADERS += mupdf/mujs/jsbuiltin.h
HEADERS += mupdf/mujs/jscompile.h
HEADERS += mupdf/mujs/jsi.h
HEADERS += mupdf/mujs/jslex.h
HEADERS += mupdf/mujs/jsparse.h
HEADERS += mupdf/mujs/jsrun.h
HEADERS += mupdf/mujs/jsvalue.h
HEADERS += mupdf/mujs/mujs.h
HEADERS += mupdf/mujs/opnames.h
HEADERS += mupdf/mujs/regexp.h
HEADERS += mupdf/mujs/utf.h

SOURCES += mupdf/mujs/one.c

# MuPDF
# =====
HEADERS += mupdf/include/mupdf/*.h mupdf/include/mupdf/fitz/*.h mupdf/include/mupdf/helpers/*.h mupdf/include/mupdf/pdf/*.h
HEADERS += mupdf/source/fitz/*.h mupdf/source/html/*.h mupdf/source/pdf/*.h mupdf/source/svg/*.h mupdf/source/xps/*.h

SOURCES += mupdf/source/cbz/*.c
SOURCES += mupdf/source/fitz/*.c
SOURCES += mupdf/source/gprf/*.c
SOURCES += mupdf/source/html/*.c
SOURCES += mupdf/source/pdf/*.c
SOURCES += mupdf/source/svg/*.c
SOURCES += mupdf/source/xps/*.c
SOURCES += mupdf/source/generated/*.c

# PdfPeep Sources
# ===============
HEADERS += Global.hpp
HEADERS += NBPdfPlugin.hpp
HEADERS += PdfPeep/MuPdfDocument.hpp
HEADERS += PdfPeep/NBPdfPeep.hpp
HEADERS += PdfPeep/PdfDocument.hpp
HEADERS += PdfPeep/PdfView.hpp
HEADERS += PdfPeep/PopplerDocument.hpp

SOURCES += NBPdfPlugin.cpp
SOURCES += PdfPeep/MuPdfDocument.cpp
SOURCES += PdfPeep/NBPdfPeep.cpp
SOURCES += PdfPeep/PdfView.cpp
SOURCES += PdfPeep/PopplerDocument.cpp

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4
UI_DIR      = $$BUILD_PREFIX/uic-plugins4

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = PdfPlugin
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/

	target.path = $$PREFIX/lib/newbreeze/plugins4
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins
	}
}

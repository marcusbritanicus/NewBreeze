TEMPLATE = lib
TARGET = DefaultPeekPlugins

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

DEPENDPATH += . ArchiveExamine DjvuDisplay ImagePeek OdfOgle PdfPeep WebWatch
INCLUDEPATH += . ArchiveExamine DjvuDisplay ImagePeek OdfOgle PdfPeep WebWatch

isEqual( QT_MAJOR_VERSION, 4 ) {
	LIBS += -L../../common/ -lnewbreeze-common
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	LIBS += -L../../common/ -lnewbreeze-common5
}

# Same as NewBreeze version
VERSION = "3.0.0"

# Needs libz
LIBS += -lz

# Needs QtXml
QT += xml

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# Plugin Mode
# ===========
CONFIG += plugin

# Webkit Support for WebWatch
# ===========================
QT += webkit
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += webkitwidgets
}

# Poppler-Qt4
# ===========
lessThan( QT_MAJOR_VERSION, 5 ) {
	INCLUDEPATH  += /usr/include/poppler/qt4
	LIBS         += -L/usr/lib -lpoppler-qt4 -ldjvulibre
}

# Poppler-Qt5
# ===========
greaterThan( QT_MAJOR_VERSION, 4 ) {
	INCLUDEPATH  += /usr/include/poppler/qt5
	LIBS         += -L/usr/lib -lpoppler-qt5 -ldjvulibre
}

# C++11/C11 Support
# =================
QMAKE_CFLAGS += -std=c++0x -w
QMAKE_CXXFLAGS += -std=c++0x -w

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins
UI_DIR      = $$BUILD_PREFIX/uic-plugins

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = DefaultPeekPlugins5
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# ========= #
#  SOURCES  #
# ========= #

## PreviewBase
## ===========
HEADERS += NBPreviewWidget.hpp
SOURCES += NBPreviewWidget.cpp

## ArchiveExamine
## ==============
HEADERS += ArchiveExamine/NBArchiveExamine.hpp ArchiveExamine/NBArchiveTreeModel.hpp ArchiveExamine/NBTreeBranch.hpp
SOURCES += ArchiveExamine/NBArchiveExamine.cpp ArchiveExamine/NBArchiveTreeModel.cpp ArchiveExamine/NBTreeBranch.cpp

## DjvuDisplay
## ===========
HEADERS += DjvuDisplay/NBDjvuDisplay.hpp
SOURCES += DjvuDisplay/NBDjvuDisplay.cpp

## ImagePeek
## =========
HEADERS += ImagePeek/NBImagePeek.hpp
SOURCES += ImagePeek/NBImagePeek.cpp

## OdfOgle
## =======
HEADERS += OdfOgle/converter.h OdfOgle/docmargin.h OdfOgle/document.h OdfOgle/formatproperty.h
HEADERS += OdfOgle/kzip.h OdfOgle/NBOdfOgle.hpp OdfOgle/styleinformation.h OdfOgle/styleparser.h
SOURCES += OdfOgle/converter.cpp OdfOgle/docmargin.cpp OdfOgle/document.cpp OdfOgle/formatproperty.cpp OdfOgle/kzip.cpp OdfOgle/NBOdfOgle.cpp
SOURCES += OdfOgle/styleinformation.cpp OdfOgle/styleparser.cpp

## PdfPeep
## =======
HEADERS += PdfPeep/NBPdfPeep.hpp
SOURCES += PdfPeep/NBPdfPeep.cpp

## WebWatch
## ========
HEADERS += WebWatch/NBWebWatch.hpp
SOURCES += WebWatch/NBWebWatch.cpp

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/

	target.path = $$PREFIX/lib/newbreeze/plugins
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins5
	}
}

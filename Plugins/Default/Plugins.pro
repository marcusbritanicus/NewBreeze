TEMPLATE = lib
TARGET = DefaultPeekPlugins

DEPENDPATH += . MimeHandler DjvuDisplay ImagePeek OdfOgle PdfPeep WebWatch WordView
INCLUDEPATH += . MimeHandler DjvuDisplay ImagePeek OdfOgle PdfPeep WebWatch WordView

# Same as NewBreeze version
VERSION = "3.0.0"

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += webkitwidgets
}

# Plugin Mode
# ===========
CONFIG += plugin


# Webkit Support for WebWatch
# ===========================
QT += webkit

# NBPreviewInterface.hpp
# ====================
NB_HEADER_PATH = $$(NB_INCLUDES)
isEmpty( NB_HEADER_PATH ) {
	error( NewBreeze plugin interface header file not detected. Please set the envronmental variable NB_INCLUDES. )
}
else {
	INCLUDEPATH += $$NB_HEADER_PATH
	DEPENDPATH += $$NB_HEADER_PATH
}

# Poppler-Qt4
# ===========
INCLUDEPATH  += /usr/include/poppler/qt4
LIBS         += -L/usr/lib -lpoppler-qt4 -ldjvulibre

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

# MimeTypes
# =========
HEADERS += MimeHandler/NBMimeDatabase.hpp MimeHandler/NBMimeDatabase_p.hpp MimeHandler/NBMimeGlobPattern_p.hpp MimeHandler/NBMimeMagicRuleMatcher_p.hpp
HEADERS += MimeHandler/NBMimeMagicRule_p.hpp MimeHandler/NBMimeProvider_p.hpp MimeHandler/NBMimeType.hpp MimeHandler/NBMimeTypeParser_p.hpp MimeHandler/NBMimeType_p.hpp
HEADERS += MimeHandler/NBStandardPaths.hpp

SOURCES += MimeHandler/NBMimeDatabase.cpp MimeHandler/NBMimeGlobPattern.cpp MimeHandler/NBMimeMagicRule.cpp MimeHandler/NBMimeMagicRuleMatcher.cpp
SOURCES += MimeHandler/NBMimeProvider.cpp MimeHandler/NBMimeType.cpp MimeHandler/NBMimeTypeParser.cpp MimeHandler/NBStandardPaths.cpp MimeHandler/NBStandardPaths_unix.cpp


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

## WordView
## ========
HEADERS += WordView/NBWordView.hpp
SOURCES += WordView/NBWordView.cpp

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	target.path = $$PREFIX/share/newbreeze/plugins
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/share/newbreeze/plugins5
	}
}

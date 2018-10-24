TEMPLATE = lib
TARGET = DefaultPeekPlugins4

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

DEPENDPATH += . DjvuDisplay ImagePeek OdfOgle WebWatch
INCLUDEPATH += . DjvuDisplay ImagePeek OdfOgle WebWatch

isEqual( QT_MAJOR_VERSION, 4 ) {
	LIBS += -L../../common/ -lnewbreeze-common4
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	QT += printsupport
	LIBS += -L../../common/ -lnewbreeze-common
}

# Same as NewBreeze version
VERSION = "3.0.0"

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

# WebEngine Support for WebWatch
# ===========================
isEqual(QT_MAJOR_VERSION, 4) {
	QT += webkit
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	lessThan( QT_MINOR_VERSION, 9 ) {
		QT += webkit
		QT += webkitwidgets
	}
	isEqual( QT_MINOR_VERSION, 9 ) | greaterThan( QT_MINOR_VERSION, 9 ) {
		QT += webengine
		QT += webenginewidgets
	}
}

# DjVulibre Support
# =================
	LIBS         += -L/usr/lib -ldjvulibre

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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4
UI_DIR      = $$BUILD_PREFIX/uic-plugins4

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = DefaultPeekPlugins
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# ========= #
#  SOURCES  #
# ========= #

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

## WebWatch
## ========
isEqual( QT_MAJOR_VERISON, 4 ) {
	HEADERS += WebWatch/NBWebWatch4.hpp
	SOURCES += WebWatch/NBWebWatch4.cpp
	HEADERS += NBPreviewWidget4.hpp
	SOURCES += NBPreviewWidget4.cpp
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	lessThan( QT_MINOR_VERSION, 9 ) {
		HEADERS += WebWatch/NBWebWatch4.hpp
		SOURCES += WebWatch/NBWebWatch4.cpp
		HEADERS += NBPreviewWidget4.hpp
		SOURCES += NBPreviewWidget4.cpp
	}
	isEqual( QT_MINOR_VERSION, 9 ) | greaterThan( QT_MINOR_VERSION, 9 ) {
		HEADERS += WebWatch/NBWebWatch.hpp
		SOURCES += WebWatch/NBWebWatch.cpp
		HEADERS += NBPreviewWidget.hpp
		SOURCES += NBPreviewWidget.cpp
	}
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

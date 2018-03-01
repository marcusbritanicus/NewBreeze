TEMPLATE = lib
TARGET = MarkDownPreview4

isEqual( QT_MAJOR_VERSION, 5 ) {
	greaterThan( QT_MINOR_VERSION, 5 ) {
		error( "This version of the is meant for Qt4, Qt 5.4 and below. Please use 'MarkDown' for Qt 5.5 and higher." )
	}
}

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

DEPENDPATH += .
INCLUDEPATH += .

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin

# Webkit Support for WebWatch
# ===========================
QT += webkit

isEqual( QT_MAJOR_VERSION, 5 ) {
	TARGET = MarkDownPreview
	QT += webkitwidgets
}

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

# Input
HEADERS += NBMarkDownPreviewWidget.hpp

SOURCES += NBMarkDownPreviewWidget.cpp

RESOURCES += markdown.qrc

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	target.path = $$PREFIX/lib/newbreeze/plugins
}

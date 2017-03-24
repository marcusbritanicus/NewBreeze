TEMPLATE = lib
TARGET = MarkDownPreview

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

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
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

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = MarkDownPreview5
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

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
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins5
	}
}

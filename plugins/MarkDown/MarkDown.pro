TEMPLATE = lib
TARGET = MarkDownPreview4

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
QT += webengine

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += webenginewidgets
	QT += webchannel
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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4
UI_DIR      = $$BUILD_PREFIX/uic-plugins4

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = MarkDownPreview
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

	target.path = $$PREFIX/lib/newbreeze/plugins4
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins
	}
}

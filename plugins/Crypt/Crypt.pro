TEMPLATE = lib
TARGET = NBCrypt

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

INCLUDEPATH += .
DEPENDPATH += .

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# C++11 Support
QMAKE_CFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++11

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
	TARGET = NBCrypt5
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# Headers
HEADERS += NBCrypt.hpp

# Sources
SOURCES += NBCrypt.cpp

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

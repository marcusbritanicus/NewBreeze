TEMPLATE = lib
TARGET = AVPlugin4

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include .
DEPENDPATH += ../../common/ ../../common/include .

# VLC Support
LIBS += -lvlc

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

# LibNB3Common
# ============
isEqual( QT_MAJOR_VERSION, 4 ) {
	LIBS += -L../../common/ -lnewbreeze-common4
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	LIBS += -L../../common/ -lnewbreeze-common
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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4/AVPlugin
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4/AVPlugin
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4/AVPlugin
UI_DIR      = $$BUILD_PREFIX/uic-plugins4/AVPlugin

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET		= AVPlugin
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5/AVPlugin
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5/AVPlugin
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5/AVPlugin
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5/AVPlugin
}

# Headers
HEADERS += Global.hpp
HEADERS += NBAVPlayer.hpp
HEADERS += NBAVPlugin.hpp

# Sources
SOURCES += NBAVPlayer.cpp
SOURCES += NBAVPlugin.cpp

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

TEMPLATE = lib
TARGET = Thumbnailer4

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include .
DEPENDPATH += ../../common/ ../../common/include .

# FFMpeg Support
# ==============
LIBS += -lffmpegthumbnailer -ldjvulibre

# Poppler-Qt4
# ===========
lessThan( QT_MAJOR_VERSION, 5 ) {
	INCLUDEPATH  += /usr/include/poppler/qt4
	LIBS         += -L/usr/lib -lpoppler-qt4
}

# Poppler-Qt5
# ===========
greaterThan( QT_MAJOR_VERSION, 4 ) {
	INCLUDEPATH  += /usr/include/poppler/qt5
	LIBS         += -L/usr/lib -lpoppler-qt5
}

# LibNB3Common
# ============
isEqual( QT_MAJOR_VERSION, 4 ) {
	LIBS += -L../../common/ -lnewbreeze-common4
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	QT += widgets
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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4
UI_DIR      = $$BUILD_PREFIX/uic-plugins4

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET		= Thumbnailer
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# Headers
HEADERS += NBThumbnailerPlugin.hpp

# Sources
SOURCES += NBThumbnailerPlugin.cpp

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

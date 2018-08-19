TEMPLATE	= app
TARGET		= MediaInfo

QT  += widgets

VERSION = 1.0.0

DEFINES += UNICODE USE_MEDIAINFO MI_TEST

INCLUDEPATH		+= . $$system(pkg-config --cflags libmediainfo libzen)
DEPENDPATH		+= . $$system(pkg-config --cflags libmediainfo libzen)

LIBS			+= $$system(pkg-config --libs libmediainfo libzen)

# HEADERS and SOURCES
HEADERS += NBMediaInfo.hpp
SOURCES += NBMediaInfo.cpp

CONFIG += silent warn_on shared_and_static

MOC_DIR		= ../build/moc
OBJECTS_DIR = ../build/objs
RCC_DIR		= ../build/qrc
UI_DIR		= ../build/uic

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS									+= target includes data
	CONFIG										+= create_pc

	contains( DEFINES, LIB64 ): target.path 	= $$INSTALL_PREFIX/lib64
	else: target.path							= $$INSTALL_PREFIX/lib

	target.path									= $$PREFIX/lib/
	includes.files								= MediaInfo.h Core.h ConfigTreeText.h
	includes.path								= $$PREFIX/include/

	data.path									= $$PREFIX/share/lib$$TARGET/
	data.files									= Changelog README

	QMAKE_PKGCONFIG_NAME						= libMediaInfo-qt
	QMAKE_PKGCONFIG_DESCRIPTION					= Qt wrapper for libmediainfo
	QMAKE_PKGCONFIG_PREFIX						= $$INSTALL_PREFIX
	QMAKE_PKGCONFIG_LIBDIR						= $$target.path
	QMAKE_PKGCONFIG_INCDIR						= $$includes.path
	QMAKE_PKGCONFIG_VERSION						= $$VERSION
	QMAKE_PKGCONFIG_DESTDIR						= pkgconfig
}

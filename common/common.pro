TEMPLATE = lib
TARGET = newbreeze-common

VERSION = 3.0.0

QT -= gui

INCLUDEPATH += . include Archive DeviceInfo MimeTypes QCryptographicHash5 StandardPaths SystemInfo Tools XDG
DEPENDPATH += . include Archive DeviceInfo MimeTypes QCryptographicHash5 StandardPaths SystemInfo Tools XDG

LIBS += -larchive -lbz2 -llzma
DEFINES += COMMON

contains ( DEFINES, HAVE_LZLIB ) {
	message( "Builting LZ compression enabled. lzip binary not required." )
	LIBS +=  -lz
}

# Headers
# =======
HEADERS += include/common.hpp
HEADERS += include/NBArchive.hpp
HEADERS += include/NBBZip2.hpp
HEADERS += include/NBDeviceInfo.hpp
HEADERS += include/NBGZip.hpp
HEADERS += include/NBLZip.hpp
HEADERS += include/NBLzma2.hpp
HEADERS += include/NBLzma.hpp
HEADERS += include/NBSystemInfo.hpp
HEADERS += include/NBTools.hpp
HEADERS += include/NBXdg.hpp
HEADERS += include/newbreeze.hpp
HEADERS += include/SimpleCrypt.hpp

# Sources
# =======
SOURCES += Archive/lz4dec.c
SOURCES += Archive/NBArchive.cpp
SOURCES += Archive/NBBZip2.cpp
SOURCES += Archive/NBGZip.cpp
SOURCES += Archive/NBLZip.cpp
SOURCES += Archive/NBLzma2.cpp
SOURCES += Archive/NBLzma.cpp
SOURCES += DeviceInfo/NBDeviceInfo.cpp
SOURCES += DeviceInfo/NBDeviceInfoUnix.cpp
SOURCES += SimpleCrypt/SimpleCrypt.cpp
SOURCES += SystemInfo/NBSystemInfo.cpp
SOURCES += Tools/NBTools.cpp
SOURCES += XDG/NBXdg.cpp

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-common5
OBJECTS_DIR = $$BUILD_PREFIX/obj-common5
RCC_DIR		= $$BUILD_PREFIX/qrc-common5
UI_DIR      = $$BUILD_PREFIX/uic-common5

# Build Shared and Static
# =======================
# CONFIG += shared_and_static build_all

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target header
	target.path = $$PREFIX/lib/newbreeze

	header.path = $$PREFIX/include/newbreeze
	header.files = $$HEADERS include/newbreeze.hpp NBPluginInterface.hpp

	# static.path = $$PREFIX/lib/newbreeze
	# static.files = libnewbreeze-common.a
}

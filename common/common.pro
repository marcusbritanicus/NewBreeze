TEMPLATE = lib
TARGET = newbreeze-common

VERSION = 3.0.0

INCLUDEPATH += . include AppFile Archive DeviceInfo QStorageInfo MimeTypes QCryptographicHash5 StandardPaths SystemInfo Tools XDG
DEPENDPATH += . include AppFile Archive DeviceInfo QStorageInfo MimeTypes QCryptographicHash5 StandardPaths SystemInfo Tools XDG

LIBS += -lmagic -larchive -lbz2 -llzma -lz
DEFINES += COMMON

# Headers
# =======
HEADERS += include/common.hpp
HEADERS += NBPluginInterface.hpp
HEADERS += include/NBAppEngine.hpp
HEADERS += include/NBAppFile.hpp
HEADERS += include/NBArchive.hpp
HEADERS += include/NBLibBZip2.hpp
HEADERS += include/NBLibGZip.hpp
HEADERS += include/NBLibLzma.hpp
HEADERS += include/NBLibLzma2.hpp
HEADERS += include/NBDeviceInfo.hpp
HEADERS += include/NBSystemInfo.hpp
HEADERS += include/NBTools.hpp
HEADERS += include/NBXdg.hpp

# Sources
# =======
SOURCES += AppFile/NBAppEngine.cpp
SOURCES += AppFile/NBAppFile.cpp
SOURCES += Archive/NBArchive.cpp
SOURCES += Archive/NBLibBZip2.cpp
SOURCES += Archive/NBLibGZip.cpp
SOURCES += Archive/NBLibLzma.cpp
SOURCES += Archive/NBLibLzma2.cpp
SOURCES += DeviceInfo/NBDeviceInfo.cpp
SOURCES += SystemInfo/NBSystemInfo.cpp
SOURCES += Tools/NBTools.cpp
SOURCES += XDG/NBXdg.cpp

# Optional Files
# ==============
lessThan(QT_MAJOR_VERSION, 5) {
	# CryptographicHash Sha3 from Qt5 v5.1
	HEADERS += include/QCryptographicHash5.hpp
	SOURCES += QCryptographicHash5/QCryptographicHash5.cpp

	# QStorageInfo from Qt5 v5.4
	HEADERS += include/QStorageInfo.hpp

	SOURCES += QStorageInfo/QStorageInfo.cpp
	SOURCES += QStorageInfo/QStorageInfo_unix.cpp

	# QMimeType from Qt5 v5.0+
	HEADERS += include/QMimeDatabase.hpp
	HEADERS += include/QMimeType.hpp
	HEADERS += include/QStandardPaths.hpp

	SOURCES += MimeTypes/QMimeDatabase.cpp
	SOURCES += MimeTypes/QMimeGlobPattern.cpp
	SOURCES += MimeTypes/QMimeMagicRule.cpp
	SOURCES += MimeTypes/QMimeMagicRuleMatcher.cpp
	SOURCES += MimeTypes/QMimeProvider.cpp
	SOURCES += MimeTypes/QMimeType.cpp
	SOURCES += MimeTypes/QMimeTypeParser.cpp
	SOURCES += StandardPaths/QStandardPaths.cpp
	SOURCES += StandardPaths/QStandardPaths_unix.cpp
}

isEqual(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 1) {
		# CryptographicHash Sha3 from Qt5 v5.1
		HEADERS += include/QCryptographicHash5.hpp
		SOURCES += QCryptographicHash5/QCryptographicHash5.cpp
	}

	lessThan(QT_MINOR_VERSION, 4) {
		# QStorageInfo from Qt5 v5.4
		HEADERS += include/QStorageInfo.hpp

		SOURCES += QStorageInfo/QStorageInfo.cpp
		SOURCES += QStorageInfo/QStorageInfo_unix.cpp
	}
}

# Resources
RESOURCES += MimeTypes/mimetypes.qrc

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-common
OBJECTS_DIR = $$BUILD_PREFIX/obj-common
RCC_DIR		= $$BUILD_PREFIX/qrc-common
UI_DIR      = $$BUILD_PREFIX/uic-common

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = newbreeze-common5
	MOC_DIR 	= $$BUILD_PREFIX/moc-common5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-common5
	RCC_DIR		= $$BUILD_PREFIX/qrc-common5
	UI_DIR      = $$BUILD_PREFIX/uic-common5
}

# Build Shared and Static
# =======================
CONFIG += shared_and_static build_all

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target static header
	target.path = $$PREFIX/lib/newbreeze

	header.path = $$PREFIX/include/newbreeze
	header.files = $$HEADERS include/newbreeze.hpp NBPluginInterface.hpp

	static.path = $$PREFIX/lib/newbreeze
	static.files = libnewbreeze-common.a
}

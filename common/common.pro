TEMPLATE = lib
TARGET = newbreeze-common

INCLUDEPATH += . AppFile Archive DeviceInfo DeviceInfo/QVolumeInfo MimeHandler QCryptographicHash5 QCryptographicHash5/md4 QCryptographicHash5/md5
INCLUDEPATH += QCryptographicHash5/rfc6234 QCryptographicHash5/sha1 QCryptographicHash5/sha3 StandardPaths SystemInfo Tools XDG

DEPENDPATH += . AppFile Archive DeviceInfo DeviceInfo/QVolumeInfo MimeHandler QCryptographicHash5 QCryptographicHash5/md4 QCryptographicHash5/md5
DEPENDPATH += QCryptographicHash5/rfc6234 QCryptographicHash5/sha1 QCryptographicHash5/sha3 StandardPaths SystemInfo Tools XDG

LIBS += -lmagic -larchive -lbz2 -llzma -lz

# Headers
# =======
HEADERS += common.hpp
HEADERS += NBPluginInterface.hpp
HEADERS += AppFile/NBAppEngine.hpp
HEADERS += AppFile/NBAppFile.hpp
HEADERS += Archive/NBArchive.hpp
HEADERS += Archive/NBLibBZip2.hpp
HEADERS += Archive/NBLibGZip.hpp
HEADERS += Archive/NBLibLzma.hpp
HEADERS += Archive/NBLibLzma2.hpp
HEADERS += DeviceInfo/NBDeviceInfo.hpp
HEADERS += SystemInfo/NBSystemInfo.hpp
HEADERS += Tools/NBTools.hpp
HEADERS += XDG/NBXdg.hpp

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
	HEADERS += QCryptographicHash5/QCryptographicHash5.hpp
	SOURCES += QCryptographicHash5/QCryptographicHash5.cpp

	# QStorageInfo from Qt5 v5.4
	HEADERS += DeviceInfo/QVolumeInfo/QVolumeInfo.hpp
	HEADERS += DeviceInfo/QVolumeInfo/QVolumeInfoP.hpp

	SOURCES += DeviceInfo/QVolumeInfo/QVolumeInfo.cpp
	SOURCES += DeviceInfo/QVolumeInfo/QVolumeInfoUnix.cpp

	# QMimeType from Qt5 v5.0+
	HEADERS += MimeHandler/NBMimeDatabase.hpp
	HEADERS += MimeHandler/NBMimeDatabase_p.hpp
	HEADERS += MimeHandler/NBMimeGlobPattern_p.hpp
	HEADERS += MimeHandler/NBMimeMagicRuleMatcher_p.hpp
	HEADERS += MimeHandler/NBMimeMagicRule_p.hpp
	HEADERS += MimeHandler/NBMimeProvider_p.hpp
	HEADERS += MimeHandler/NBMimeType.hpp
	HEADERS += MimeHandler/NBMimeTypeParser_p.hpp
	HEADERS += MimeHandler/NBMimeType_p.hpp
	HEADERS += StandardPaths/NBStandardPaths.hpp

	SOURCES += MimeHandler/NBMimeDatabase.cpp
	SOURCES += MimeHandler/NBMimeGlobPattern.cpp
	SOURCES += MimeHandler/NBMimeMagicRule.cpp
	SOURCES += MimeHandler/NBMimeMagicRuleMatcher.cpp
	SOURCES += MimeHandler/NBMimeProvider.cpp
	SOURCES += MimeHandler/NBMimeType.cpp
	SOURCES += MimeHandler/NBMimeTypeParser.cpp
	SOURCES += StandardPaths/NBStandardPaths.cpp
	SOURCES += StandardPaths/NBStandardPaths_unix.cpp
}

isEqual(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 1) {
		# CryptographicHash Sha3 from Qt5 v5.1
		HEADERS += QCryptographicHash5/QCryptographicHash5.hpp
		SOURCES += QCryptographicHash5/QCryptographicHash5.cpp
	}

	lessThan(QT_MINOR_VERSION, 4) {
		# QStorageInfo from Qt5 v5.4
		HEADERS += DeviceInfo/QVolumeInfo/QVolumeInfo.hpp
		HEADERS += DeviceInfo/QVolumeInfo/QVolumeInfoP.hpp

		SOURCES += DeviceInfo/QVolumeInfo/QVolumeInfo.cpp
		SOURCES += DeviceInfo/QVolumeInfo/QVolumeInfoUnix.cpp
	}
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

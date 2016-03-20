TEMPLATE = lib
TARGET = NBCrypt

INCLUDEPATH += . Core/Circle Core/QCryptographicHash5 Core/Salsa20 Core/Tools Core/QVolumeInfo Gui/Common Gui/EncFS Gui/Salsa icons

DEPENDPATH += . Core/Circle Core/QCryptographicHash5 Core/Salsa20 Core/Tools Core/QVolumeInfo Gui/Common Gui/EncFS Gui/Salsa icons

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

# NBPreviewInterface.hpp
# ====================
NB_HEADER_PATH = $$(NB_INCLUDES)
isEmpty( NB_HEADER_PATH ) {
	error( NewBreeze plugin interface header file not detected. Please set the envronmental variable NB_INCLUDES. )
}
else {
	INCLUDEPATH += $$NB_HEADER_PATH
	DEPENDPATH += $$NB_HEADER_PATH
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
	TARGET = MarkDownPreview5
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# Headers
HEADERS += Core/Circle/NBCircle.hpp
HEADERS += Core/Tools/NBTools.hpp
HEADERS += Gui/Common/NBPasswordDialog.hpp
HEADERS += Gui/Common/NBPasswordEdit.hpp
HEADERS += Gui/Common/NBPasswordInput.hpp
HEADERS += Gui/Common/NBPatternPad.hpp
HEADERS += Gui/EncFS/NBEncFS.hpp
HEADERS += Gui/EncFS/NBNewEncFS.hpp
HEADERS += Gui/Salsa/NBSalsa20.hpp
HEADERS += NBCrypt.hpp

# Sources
SOURCES += Core/Circle/NBCircle.cpp
SOURCES += Core/Tools/NBTools.cpp
SOURCES += Gui/Common/NBPasswordDialog.cpp
SOURCES += Gui/Common/NBPasswordEdit.cpp
SOURCES += Gui/Common/NBPasswordInput.cpp
SOURCES += Gui/Common/NBPatternPad.cpp
SOURCES += Gui/EncFS/NBEncFS.cpp
SOURCES += Gui/EncFS/NBNewEncFS.cpp
SOURCES += Gui/Salsa/NBSalsa20.cpp
SOURCES += NBCrypt.cpp

lessThan(QT_MAJOR_VERSION, 5) {
	# CryptographicHash Sha3 from Qt5 v5.1
	HEADERS += Core/QCryptographicHash5/QCryptographicHash5.hpp
	SOURCES += Core/QCryptographicHash5/QCryptographicHash5.cpp

	# QStorageInfo from Qt5 v5.4
	HEADERS += Core/QVolumeInfo/QVolumeInfo.hpp
	HEADERS += Core/QVolumeInfo/QVolumeInfoP.hpp

	SOURCES += Core/QVolumeInfo/QVolumeInfo.cpp
	SOURCES += Core/QVolumeInfo/QVolumeInfoUnix.cpp
}

isEqual(QT_MAJOR_VERSION, 5) {
	lessThan(QT_MINOR_VERSION, 1) {
		# CryptographicHash Sha3 from Qt5 v5.1
		HEADERS += Core/QCryptographicHash5/QCryptographicHash5.hpp
		SOURCES += Core/QCryptographicHash5/QCryptographicHash5.cpp
	}

	lessThan(QT_MINOR_VERSION, 4) {
		# QStorageInfo from Qt5 v5.4
		HEADERS += Core/QVolumeInfo/QVolumeInfo.hpp
		HEADERS += Core/QVolumeInfo/QVolumeInfoP.hpp

		SOURCES += Core/QVolumeInfo/QVolumeInfo.cpp
		SOURCES += Core/QVolumeInfo/QVolumeInfoUnix.cpp
	}
}

# Icon and stylesheet resources
RESOURCES += Crypt.qrc

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

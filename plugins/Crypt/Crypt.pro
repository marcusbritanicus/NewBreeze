TEMPLATE = lib
TARGET = NBCrypt

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

INCLUDEPATH += . Core/Circle Core/Salsa20 Gui/Common Gui/EncFS Gui/Salsa icons
DEPENDPATH += . Core/Circle Core/Salsa20 Gui/Common Gui/EncFS Gui/Salsa icons

isEqual( QT_MAJOR_VERSION, 4 ) {
	LIBS += -L../../common/ -lnewbreeze-common
}

isEqual( QT_MAJOR_VERSION, 5 ) {
	LIBS += -L../../common/ -lnewbreeze-common5
}

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
HEADERS += Core/Circle/NBCircle.hpp
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
SOURCES += Gui/Common/NBPasswordDialog.cpp
SOURCES += Gui/Common/NBPasswordEdit.cpp
SOURCES += Gui/Common/NBPasswordInput.cpp
SOURCES += Gui/Common/NBPatternPad.cpp
SOURCES += Gui/EncFS/NBEncFS.cpp
SOURCES += Gui/EncFS/NBNewEncFS.cpp
SOURCES += Gui/Salsa/NBSalsa20.cpp
SOURCES += NBCrypt.cpp

# Icon and stylesheet resources
RESOURCES += Crypt.qrc

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

TEMPLATE = app
TARGET = NBVault

QT += widgets

INCLUDEPATH += . ../../common/include EncFS KeyDialog Salsa20

greaterThan(QT_MAJOR_VERSION, 4) {
	Qt += widgets
	LIBS += -L../../common/ -lnewbreeze-common4
}

isEqual(QT_MAJOR_VERSION, 4) {
	LIBS += -L../../common/ -lnewbreeze-common
}

# Input
HEADERS += NBCircle.hpp
HEADERS += NBVault.hpp
HEADERS += NBVaultDatabase.hpp
HEADERS += EncFS/NBEncFS.hpp
HEADERS += KeyDialog/NBPasswordDialog.hpp
HEADERS += KeyDialog/NBPasswordEdit.hpp
HEADERS += KeyDialog/NBPasswordInput.hpp
HEADERS += KeyDialog/NBPatternPad.hpp
HEADERS += Salsa20/NBSalsa20.hpp
HEADERS += Salsa20/Salsa20.hpp

SOURCES += NBCircle.cpp
SOURCES += Main.cpp
SOURCES += NBVault.cpp
SOURCES += NBVaultDatabase.cpp
SOURCES += KeyDialog/NBPasswordDialog.cpp
SOURCES += KeyDialog/NBPasswordEdit.cpp
SOURCES += KeyDialog/NBPasswordInput.cpp
SOURCES += KeyDialog/NBPatternPad.cpp
SOURCES += EncFS/NBEncFS.cpp
SOURCES += Salsa20/NBSalsa20.cpp

CONFIG += silent

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}
	BINDIR = $$PREFIX/bin

	INSTALLS += target
	target.path = $$BINDIR

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/
}

# Build location

BUILD_PREFIX = $$(NB_BUILD_DIR)

isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-vault4
OBJECTS_DIR = $$BUILD_PREFIX/obj-vault4
RCC_DIR		= $$BUILD_PREFIX/qrc-vault4
UI_DIR      = $$BUILD_PREFIX/uic-vault4

greaterThan(QT_MAJOR_VERSION, 4) {
	MOC_DIR 	= $$BUILD_PREFIX/moc-vault5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-vault5
	RCC_DIR		= $$BUILD_PREFIX/qrc-vault5
	UI_DIR      = $$BUILD_PREFIX/uic-vault5
}

DEFINES += STANDALONE

TEMPLATE = lib
TARGET = TermPlugin4

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

# Common Sources
DEPENDPATH += . lib ../../common/ ../../common/include
INCLUDEPATH += . lib ../../common/ ../../common/include

# NBTerminal Sources
SOURCES += NBTerminal.cpp
SOURCES += NBTermPlugin.cpp
SOURCES += NBTermWidget.cpp
SOURCES += NBTSettingsDialog.cpp
SOURCES += NBTTools.cpp
SOURCES += TtyTabWidget.cpp

# NBTerminal Headers
HEADERS += Global.hpp
HEADERS += NBTerminal.hpp
HEADERS += NBTermPlugin.hpp
HEADERS += NBTermWidget.hpp
HEADERS += NBTSettingsDialog.hpp
HEADERS += NBTTools.hpp
HEADERS += TtyTabWidget.hpp

# QTermWidget Sources
SOURCES += lib/BlockArray.cpp
SOURCES += lib/ColorScheme.cpp
SOURCES += lib/Emulation.cpp
SOURCES += lib/Filter.cpp
SOURCES += lib/History.cpp
SOURCES += lib/HistorySearch.cpp
SOURCES += lib/KeyboardTranslator.cpp
SOURCES += lib/konsole_wcwidth.cpp
SOURCES += lib/kprocess.cpp
SOURCES += lib/kpty.cpp
SOURCES += lib/kptydevice.cpp
SOURCES += lib/kptyprocess.cpp
SOURCES += lib/Pty.cpp
SOURCES += lib/qtermwidget.cpp
SOURCES += lib/Screen.cpp
SOURCES += lib/ScreenWindow.cpp
SOURCES += lib/Session.cpp
SOURCES += lib/ShellCommand.cpp
SOURCES += lib/TerminalCharacterDecoder.cpp
SOURCES += lib/TerminalDisplay.cpp
SOURCES += lib/tools.cpp
SOURCES += lib/Vt102Emulation.cpp

# QTermWidget Headers
HEADERS += lib/BlockArray.h
HEADERS += lib/CharacterColor.h
HEADERS += lib/Character.h
HEADERS += lib/ColorScheme.h
HEADERS += lib/ColorTables.h
HEADERS += lib/DefaultTranslatorText.h
HEADERS += lib/Emulation.h
HEADERS += lib/ExtendedDefaultTranslator.h
HEADERS += lib/Filter.h
HEADERS += lib/History.h
HEADERS += lib/HistorySearch.h
HEADERS += lib/KeyboardTranslator.h
HEADERS += lib/konsole_wcwidth.h
HEADERS += lib/kprocess.h
HEADERS += lib/kptydevice.h
HEADERS += lib/kpty.h
HEADERS += lib/kpty_p.h
HEADERS += lib/kptyprocess.h
HEADERS += lib/LineFont.h
HEADERS += lib/Pty.h
HEADERS += lib/qtermwidget.h
HEADERS += lib/Screen.h
HEADERS += lib/ScreenWindow.h
HEADERS += lib/Session.h
HEADERS += lib/ShellCommand.h
HEADERS += lib/TerminalCharacterDecoder.h
HEADERS += lib/TerminalDisplay.h
HEADERS += lib/tools.h
HEADERS += lib/Vt102Emulation.h

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
	TARGET = TermPlugin
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

DEFINES += "HAVE_POSIX_OPENPT"
DEFINES += "HAVE_SYS_TIME_H"

RESOURCES += NBTerminal.qrc
CONFIG += silent

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/
	target.path = $$PREFIX/lib/newbreeze/plugins
}

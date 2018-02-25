TEMPLATE = lib
TARGET = TextEditor4

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin qscintilla2

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

INCLUDEPATH += . Gui/Editor Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps Gui/Widgets/Misc Gui/Widgets/SearchReplace
INCLUDEPATH += Gui/UI Resources Resources/icons

DEPENDPATH += . Gui/Editor Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps Gui/Widgets/Misc Gui/Widgets/SearchReplace
DEPENDPATH += Gui/UI Resources Resources/icons

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += printsupport
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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4
UI_DIR      = $$BUILD_PREFIX/uic-plugins4

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = TextEditor
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# Headers
HEADERS += Global.hpp
HEADERS += Gui/Editor/QsciEditor.hpp
HEADERS += Gui/Editor/QsciLexer.hpp
HEADERS += Gui/Editor/QsciLexerLaTeX.hpp
HEADERS += Gui/Widgets/Buttons/NBWindowButton.hpp
HEADERS += Gui/Widgets/Buttons/ButtonSet.hpp
HEADERS += Gui/Widgets/FileProps/FileProps.hpp
HEADERS += Gui/Widgets/Misc/GuiWidgets.hpp
HEADERS += Gui/Widgets/SearchReplace/SR.hpp
HEADERS += Gui/UI/TextEditor.hpp
HEADERS += NBTextEditPlugin.hpp

# Sources
SOURCES += Gui/Editor/QsciEditor.cpp
SOURCES += Gui/Editor/QsciLexer.cpp
SOURCES += Gui/Editor/QsciLexerLaTeX.cpp
SOURCES += Gui/Widgets/Buttons/NBWindowButton.cpp
SOURCES += Gui/Widgets/Buttons/ButtonSet.cpp
SOURCES += Gui/Widgets/FileProps/FileProps.cpp
SOURCES += Gui/Widgets/Misc/GuiWidgets.cpp
SOURCES += Gui/Widgets/SearchReplace/SR.cpp
SOURCES += Gui/UI/TextEditor.cpp
SOURCES += NBTextEditPlugin.cpp

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	target.path = $$PREFIX/lib/newbreeze/plugins
}

TEMPLATE = lib
TARGET = TextEditor

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

INCLUDEPATH += . Gui/Editor Gui/Editor/Syntaxer Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps Gui/Widgets/Misc Gui/Widgets/SearchReplace
INCLUDEPATH += Gui/UI Resources Resources/icons

DEPENDPATH += . Gui/Editor Gui/Editor/Syntaxer Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps Gui/Widgets/Misc Gui/Widgets/SearchReplace
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

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins
UI_DIR      = $$BUILD_PREFIX/uic-plugins

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = TextEditor5
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5
}

# Headers
HEADERS += Global.hpp
HEADERS += Gui/Editor/Editor.hpp
HEADERS += Gui/Editor/Syntaxer/GNUSyntaxHighlighter.h
HEADERS += Gui/Editor/Syntaxer/HighlightStateData.h
HEADERS += Gui/Editor/Syntaxer/LanguageComboBox.h
HEADERS += Gui/Editor/Syntaxer/ParagraphMap.h
HEADERS += Gui/Editor/Syntaxer/Qt4SourceHighlightStyleGenerator.h
HEADERS += Gui/Editor/Syntaxer/Qt4SyntaxHighlighter.h
HEADERS += Gui/Editor/Syntaxer/Qt4TextFormatterFactory.h
HEADERS += Gui/Editor/Syntaxer/Qt4TextFormatter.h
HEADERS += Gui/Editor/Syntaxer/QtColorMap.h
HEADERS += Gui/Editor/Syntaxer/SourceHighlightExceptionBox.h
HEADERS += Gui/Editor/Syntaxer/StyleComboBox.h
HEADERS += Gui/Editor/Syntaxer/TextEditHighlighted.h
HEADERS += Gui/Editor/Syntaxer/TextFormatterFactory.h
HEADERS += Gui/Editor/Syntaxer/TextFormatter.h
HEADERS += Gui/Widgets/Buttons/NBWindowButton.hpp
HEADERS += Gui/Widgets/Buttons/ButtonSet.hpp
HEADERS += Gui/Widgets/FileProps/FileProps.hpp
HEADERS += Gui/Widgets/Misc/GuiWidgets.hpp
HEADERS += Gui/Widgets/SearchReplace/SR.hpp
HEADERS += Gui/UI/TextEditor.hpp
HEADERS += NBTextEditPlugin.hpp

# Sources
SOURCES += Gui/Editor/Editor.cpp
SOURCES += Gui/Editor/Syntaxer/GNUSyntaxHighlighter.cpp
SOURCES += Gui/Editor/Syntaxer/LanguageComboBox.cpp
SOURCES += Gui/Editor/Syntaxer/ParagraphMap.cpp
SOURCES += Gui/Editor/Syntaxer/Qt4SourceHighlightStyleGenerator.cpp
SOURCES += Gui/Editor/Syntaxer/Qt4SyntaxHighlighter.cpp
SOURCES += Gui/Editor/Syntaxer/Qt4TextFormatter.cpp
SOURCES += Gui/Editor/Syntaxer/Qt4TextFormatterFactory.cpp
SOURCES += Gui/Editor/Syntaxer/QtColorMap.cpp
SOURCES += Gui/Editor/Syntaxer/SourceHighlightExceptionBox.cpp
SOURCES += Gui/Editor/Syntaxer/StyleComboBox.cpp
SOURCES += Gui/Editor/Syntaxer/TextEditHighlighted.cpp
SOURCES += Gui/Editor/Syntaxer/TextFormatter.cpp
SOURCES += Gui/Editor/Syntaxer/TextFormatterFactory.cpp
SOURCES += Gui/Widgets/Buttons/NBWindowButton.cpp
SOURCES += Gui/Widgets/Buttons/ButtonSet.cpp
SOURCES += Gui/Widgets/FileProps/FileProps.cpp
SOURCES += Gui/Widgets/Misc/GuiWidgets.cpp
SOURCES += Gui/Widgets/SearchReplace/SR.cpp
SOURCES += Gui/UI/TextEditor.cpp
SOURCES += NBTextEditPlugin.cpp

# Pkg-Config for source-higlight
CONFIG += link_pkgconfig
PKGCONFIG += source-highlight

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

DEFINES += "HAVE_GNU_SYNTAX_HILITE"

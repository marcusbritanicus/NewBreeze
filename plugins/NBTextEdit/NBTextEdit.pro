TEMPLATE = lib
TARGET = TextEditor

# Same as NewBreeze version
VERSION = "3.0.0"

# Plugin Mode
# ===========
CONFIG += plugin

INCLUDEPATH += . Core Core/MimeHandler Core/StandardPaths Core/Tools Gui/Editor Gui/Editor/Syntaxer Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps
INCLUDEPATH += Gui/Widgets/Misc Gui/Widgets/SearchReplace Gui/WordyUI Resources Resources/icons

DEPENDPATH += . Core Core/MimeHandler Core/StandardPaths Core/Tools Gui/Editor Gui/Editor/Syntaxer Gui/Application Gui/Widgets Gui/Widgets/Buttons Gui/Widgets/FileProps
DEPENDPATH += Gui/Widgets/Misc Gui/Widgets/SearchReplace Gui/WordyUI Resources Resources/icons

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
	QT += printsupport
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
HEADERS += Core/Tools/Tools.hpp
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
HEADERS += Gui/WordyUI/Wordy.hpp
HEADERS += NBTextEditPlugin.hpp

# Sources
SOURCES += Core/Tools/Tools.cpp
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
SOURCES += Gui/WordyUI/Wordy.cpp
SOURCES += NBTextEditPlugin.cpp

# NBMimeDatabase
lessThan( QT_MAJOR_VERSION, 5 ) {
	HEADERS += Core/MimeHandler/NBMimeDatabase.hpp
	HEADERS += Core/MimeHandler/NBMimeDatabase_p.hpp
	HEADERS += Core/MimeHandler/NBMimeGlobPattern_p.hpp
	HEADERS += Core/MimeHandler/NBMimeMagicRuleMatcher_p.hpp
	HEADERS += Core/MimeHandler/NBMimeMagicRule_p.hpp
	HEADERS += Core/MimeHandler/NBMimeProvider_p.hpp
	HEADERS += Core/MimeHandler/NBMimeType.hpp
	HEADERS += Core/MimeHandler/NBMimeTypeParser_p.hpp
	HEADERS += Core/MimeHandler/NBMimeType_p.hpp
	HEADERS += Core/StandardPaths/NBStandardPaths.hpp
	SOURCES += Core/MimeHandler/NBMimeDatabase.cpp
	SOURCES += Core/MimeHandler/NBMimeGlobPattern.cpp
	SOURCES += Core/MimeHandler/NBMimeMagicRule.cpp
	SOURCES += Core/MimeHandler/NBMimeMagicRuleMatcher.cpp
	SOURCES += Core/MimeHandler/NBMimeProvider.cpp
	SOURCES += Core/MimeHandler/NBMimeType.cpp
	SOURCES += Core/MimeHandler/NBMimeTypeParser.cpp
	SOURCES += Core/StandardPaths/NBStandardPaths.cpp
	SOURCES += Core/StandardPaths/NBStandardPaths_unix.cpp
}

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

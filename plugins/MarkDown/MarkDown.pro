TEMPLATE = lib
TARGET = MarkDownPreview4

# Common Sources
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

DEPENDPATH += .
INCLUDEPATH += . cmark

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

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4/Markdown
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4/Markdown
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4/Markdown
UI_DIR      = $$BUILD_PREFIX/uic-plugins4/Markdown

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = MarkDownPreview
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5/Markdown
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5/Markdown
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5/Markdown
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5/Markdown
}

# Input
HEADERS += NBMarkDownPreviewWidget.hpp
SOURCES += NBMarkDownPreviewWidget.cpp

# CMark Sources
SOURCES += cmark/blocks.c
SOURCES += cmark/buffer.c
SOURCES += cmark/cmark.c
SOURCES += cmark/cmark_ctype.c
SOURCES += cmark/commonmark.c
SOURCES += cmark/houdini_href_e.c
SOURCES += cmark/houdini_html_e.c
SOURCES += cmark/houdini_html_u.c
SOURCES += cmark/html.c
SOURCES += cmark/inlines.c
SOURCES += cmark/iterator.c
SOURCES += cmark/latex.c
SOURCES += cmark/man.c
SOURCES += cmark/node.c
SOURCES += cmark/references.c
SOURCES += cmark/render.c
SOURCES += cmark/scanners.c
SOURCES += cmark/utf8.c
SOURCES += cmark/xml.c

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	target.path = $$PREFIX/lib/newbreeze/plugins4
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins
	}
}

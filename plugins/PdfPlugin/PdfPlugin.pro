TEMPLATE = lib
TARGET = PdfPlugin4

# Same as NewBreeze version
VERSION = "3.0.0"

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets  widgets-private
}

# Common Locations
# ================
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

# PdfView
# =======
INCLUDEPATH += PdfView
DEPENDPATH += PdfView

# Poppler
# =======
equals( QT_MAJOR_VERSION, 4 ) {
	INCLUDEPATH  += /usr/include/poppler/qt4
	LIBS += -lpoppler-qt4
}

equals( QT_MAJOR_VERSION, 5 ) {
	INCLUDEPATH  += /usr/include/poppler/qt5
	LIBS += -lpoppler-qt5
}

# Silent and No warning
# =====================
CONFIG += warn_off silent

# Plugin Mode
# ===========
CONFIG += plugin

# PdfView Sources
# ===============
HEADERS += Global.hpp
HEADERS += NBPdfPlugin.hpp
HEADERS += PdfView/NBPdfPeep.hpp
HEADERS += PdfView/qpdfdocument.h
HEADERS += PdfView/qpdfnamespace.h
HEADERS += PdfView/qpdfpagenavigation.h
HEADERS += PdfView/qpdfpagerenderer.h
HEADERS += PdfView/qpdfview.h

SOURCES += NBPdfPlugin.cpp
SOURCES += PdfView/NBPdfPeep.cpp
SOURCES += PdfView/qpdfdocument.cpp
SOURCES += PdfView/qpdfpagenavigation.cpp
SOURCES += PdfView/qpdfpagerenderer.cpp
SOURCES += PdfView/qpdfview.cpp

# Silent compilation
# ==================
CONFIG += silent

# Build location
# ==============
BUILD_PREFIX = $$(NB_BUILD_DIR)
isEmpty( BUILD_PREFIX ) {
	BUILD_PREFIX = ./build
}

MOC_DIR 	= $$BUILD_PREFIX/moc-plugins4/PdfPlugin
OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins4/PdfPlugin
RCC_DIR		= $$BUILD_PREFIX/qrc-plugins4/PdfPlugin
UI_DIR      = $$BUILD_PREFIX/uic-plugins4/PdfPlugin

greaterThan(QT_MAJOR_VERSION, 4) {
	TARGET = PdfPlugin
	MOC_DIR 	= $$BUILD_PREFIX/moc-plugins5/PdfPlugin
	OBJECTS_DIR = $$BUILD_PREFIX/obj-plugins5/PdfPlugin
	RCC_DIR		= $$BUILD_PREFIX/qrc-plugins5/PdfPlugin
	UI_DIR      = $$BUILD_PREFIX/uic-plugins5/PdfPlugin
}

unix {
	isEmpty(PREFIX) {
		PREFIX = /usr
	}

	INSTALLS += target

	QMAKE_RPATHDIR += $$PREFIX/lib/newbreeze/

	target.path = $$PREFIX/lib/newbreeze/plugins4
	greaterThan(QT_MAJOR_VERSION, 4) {
		target.path = $$PREFIX/lib/newbreeze/plugins
	}
}

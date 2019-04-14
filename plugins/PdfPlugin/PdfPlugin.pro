TEMPLATE = lib
TARGET = PdfPlugin4

# Same as NewBreeze version
VERSION = "3.0.0"

# Qt5 Support
# ===========
greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

# Common Locations
# ================
INCLUDEPATH += ../../common/ ../../common/include
DEPENDPATH += ../../common/ ../../common/include

# MuPDF and PdfPeep
# =================
INCLUDEPATH += PdfPeep
DEPENDPATH += PdfPeep

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

# PdfPeep Sources
# ===============
HEADERS += Global.hpp
HEADERS += NBPdfPlugin.hpp
# HEADERS += PdfPeep/MuPdfDocument.hpp
HEADERS += PdfPeep/NBPdfPeep.hpp
HEADERS += PdfPeep/PdfDocument.hpp
HEADERS += PdfPeep/PdfView.hpp
HEADERS += PdfPeep/PopplerDocument.hpp

SOURCES += NBPdfPlugin.cpp
# SOURCES += PdfPeep/MuPdfDocument.cpp
SOURCES += PdfPeep/NBPdfPeep.cpp
SOURCES += PdfPeep/PdfView.cpp
SOURCES += PdfPeep/PopplerDocument.cpp

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

TEMPLATE = app
TARGET = epeg
INCLUDEPATH += .

# Input
HEADERS += Epeg.h epeg_private.h
SOURCES += epeg_main.c main.cpp saveJpegThumb.cpp

LIBS += -lexif -ljpeg

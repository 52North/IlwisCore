#-------------------------------------------------
#
# Project created by QtCreator 2013-03-29T09:57:39
#
#-------------------------------------------------

CONFIG += plugin

QT       -= gui

TARGET = featureoperations
TEMPLATE = lib

include(global.pri)

DEFINES += FEATUREOPERATIONS_LIBRARY

HEADERS += \
    featureoperations/featureoperationsmodule.h \
    featureoperations/gridding.h \
    featureoperations/pointrastercrossing.h \
    featureoperations/transformcoordinates.h \
    featureoperations/buffer.h \
    featureoperations/polygon2line.h \
    featureoperations/line2polygon.h \
    featureoperations/raster2point.h

SOURCES += \
    featureoperations/featureoperationsmodule.cpp \
    featureoperations/gridding.cpp \
    featureoperations/pointrastercrossing.cpp \
    featureoperations/transformcoordinates.cpp \
    featureoperations/buffer.cpp \
    featureoperations/polygon2line.cpp \
    featureoperations/line2polygon.cpp \
    featureoperations/raster2point.cpp

OTHER_FILES += \ 
    featureoperations/featureoperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

linux{
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target
}

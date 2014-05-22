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
    featureoperations/gridding.h

SOURCES += \
    featureoperations/featureoperationsmodule.cpp \
    featureoperations/gridding.cpp

OTHER_FILES += \ 
    featureoperations/featureoperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

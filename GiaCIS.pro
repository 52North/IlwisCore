#-------------------------------------------------
#
# Project created by QtCreator 2016-02-09T13:41:56
#
#-------------------------------------------------

CONFIG += plugin

QT       -= gui

TARGET = GiaCIS
TEMPLATE = lib

include(global.pri)

DEFINES += GIACIS_LIBRARY

SOURCES += GiaCIS/giacis_module.cpp \
    GiaCIS/percentilefilterstretch.cpp \
    GiaCIS/seasonpercentageaggregate.cpp \
    GiaCIS/aggregationtime.cpp

HEADERS += GiaCIS/giacis_module.h \
    GiaCIS/percentilefilterstretch.h \
    GiaCIS/seasonpercentageaggregate.h \
    GiaCIS/aggregationtime.h \
    GiaCIS/zoneutil.h

OTHER_FILES += \
    giacis/giacis.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET


lunix {
    target.path = /usr/lib
    INSTALLS += target
}

#-------------------------------------------------
#
# Project created by QtCreator 2014-07-14T08:25:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

include(global.pri)

TARGET = ilwis
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += commandlineclient/main.cpp

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}


linux{
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/
    INSTALLS += target
}

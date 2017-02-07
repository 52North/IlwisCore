#-------------------------------------------------
#
# Project created by QtCreator 2016-10-31T13:41:56
#
#-------------------------------------------------

CONFIG += plugin

QT       -= gui

TARGET = Hydroflow
TEMPLATE = lib

include(global.pri)

DEFINES += HYDROFLOW_LIBRARY

SOURCES += Hydroflow/hydroflow_module.cpp \
    Hydroflow/relativedem.cpp \
    Hydroflow/normalizerelativedem.cpp


HEADERS += Hydroflow/hydroflow_global.h \
    Hydroflow/hydroflow_module.h \
    Hydroflow/relativedem.h \
    Hydroflow/normalizerelativedem.h

OTHER_FILES += \
    Hydroflow/hydroflow.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET


lunix {
    target.path = /usr/lib
    INSTALLS += target
}

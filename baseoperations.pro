#-------------------------------------------------
#
# Project created by QtCreator 2013-04-22T08:25:30
#
#-------------------------------------------------

QT       += sql

QT       -= gui

CONFIG += plugin
TARGET = baseoperations
TEMPLATE = lib

include(global.pri)

DEFINES += baseoperations_LIBRARY

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

HEADERS += \
    baseoperations/baseoperationsmodule.h \
    baseoperations/baseoperations_global.h \
    baseoperations/math/binarymath.h \
    baseoperations/geometry/resampleraster.h \
    baseoperations/data/rasvalue.h \
    baseoperations/data/iffraster.h \
    baseoperations/math/binarylogical.h \
    baseoperations/math/unarymath.h \
    baseoperations/math/unarymathoperations.h

SOURCES += \
    baseoperations/baseoperationsmodule.cpp \
    baseoperations/math/binarymath.cpp \
    baseoperations/geometry/resampleraster.cpp \
    baseoperations/data/rasvalue.cpp \
    baseoperations/data/iffraster.cpp \
    baseoperations/math/binarylogical.cpp \
    baseoperations/math/unarymath.cpp \
    baseoperations/math/unarymathoperations.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

OTHER_FILES += \
    baseoperations/baseoperations.json

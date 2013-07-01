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
    baseoperations/math/unarymathoperations.h \
    baseoperations/data/assignment.h \
    baseoperations/data/selection.h \
    baseoperations/geometry/pixel2coord.h \
    baseoperations/geometry/fpixel.h \
    baseoperations/geometry/fcoordinate.h \
    baseoperations/geometry/coord2pixel.h \
    baseoperations/util/text2output.h \
    baseoperations/geometry/gridsize.h \
    baseoperations/util/stringoperations.h \
    baseoperations/data/ifoperation.h \
    baseoperations/data/iffeature.h \
    baseoperations/data/selectionfeatures.h

SOURCES += \
    baseoperations/baseoperationsmodule.cpp \
    baseoperations/math/binarymath.cpp \
    baseoperations/geometry/resampleraster.cpp \
    baseoperations/data/rasvalue.cpp \
    baseoperations/data/iffraster.cpp \
    baseoperations/math/binarylogical.cpp \
    baseoperations/math/unarymath.cpp \
    baseoperations/math/unarymathoperations.cpp \
    baseoperations/data/assignment.cpp \
    baseoperations/data/selection.cpp \
    baseoperations/geometry/pixel2coord.cpp \
    baseoperations/geometry/fpixel.cpp \
    baseoperations/geometry/fcoordinate.cpp \
    baseoperations/geometry/coord2pixel.cpp \
    baseoperations/util/text2output.cpp \
    baseoperations/geometry/gridsize.cpp \
    baseoperations/util/stringoperations.cpp \
    baseoperations/data/ifoperation.cpp \
    baseoperations/data/iffeature.cpp \
    baseoperations/data/selectionfeatures.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

OTHER_FILES += \
    baseoperations/baseoperations.json

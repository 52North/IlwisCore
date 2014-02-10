#-------------------------------------------------
#
# Project created by QtCreator 2013-04-22T08:25:30
#
#-------------------------------------------------
CONFIG += plugin
TARGET = baseoperations

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       += sql

QT       -= gui

TEMPLATE = lib

DEFINES += BASEOPERATIONS_LIBRARY

HEADERS += \
    baseoperations/baseoperationsmodule.h \
    baseoperations/baseoperations_global.h \
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
    baseoperations/data/selectionfeatures.h \
    baseoperations/math/binarymathraster.h \
    baseoperations/math/binarymathfeature.h \
    baseoperations/geometry/mastergeoreference.h \
    baseoperations/data/selectiontable.h \
    baseoperations/math/binarymathtable.h \
    baseoperations/data/setvaluerange.h

SOURCES += \
    baseoperations/baseoperationsmodule.cpp \
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
    baseoperations/data/selectionfeatures.cpp \
    baseoperations/math/binarymathraster.cpp \
    baseoperations/math/binarymathfeature.cpp \
    baseoperations/geometry/mastergeoreference.cpp \
    baseoperations/data/selectiontable.cpp \
    baseoperations/math/binarymathtable.cpp \
    baseoperations/data/setvaluerange.cpp

win32:CONFIG(release, debug|release): {
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
    LIBS += -L$$PWD/../libraries/win32release/ -llibgeos
    QMAKE_CXXFLAGS_RELEASE += -O2
}
else:win32:CONFIG(debug, debug|release): {
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
    LIBS += -L$$PWD/../libraries/win32debug/ -llibgeos
}

INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos

OTHER_FILES += \
    baseoperations/baseoperations.json

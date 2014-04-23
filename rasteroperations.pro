#-------------------------------------------------
#
# Project created by QtCreator 2013-03-29T09:57:39
#
#-------------------------------------------------

CONFIG += plugin

QT       -= gui

TARGET = rasteroperations
TEMPLATE = lib

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

DEFINES += RASTEROPERATIONS_LIBRARY

SOURCES += \ 
    rasteroperations/rasteroperationsmodule.cpp \
    rasteroperations/aggregateraster.cpp \
    rasteroperations/crossrasters.cpp \
    rasteroperations/rasterstretchoperation.cpp \
    rasteroperations/classification/rasterclassification.cpp \
    rasteroperations/classification/classifier.cpp \
    rasteroperations/classification/rasterslicing.cpp \
    rasteroperations/classification/areanumbering.cpp \
    rasteroperations/filter/rankorderrasterfilter.cpp \
    rasteroperations/filter/linearrasterfilter.cpp


HEADERS += \
    rasteroperations/rasteroperationsmodule.h \
    rasteroperations/aggregateraster.h \
    rasteroperations/crossrasters.h \
    rasteroperations/rasterstrechoperation.h \
    rasteroperations/classification/rasterclassification.h \
    rasteroperations/classification/classifier.h \
    rasteroperations/classification/rasterslicing.h \
    rasteroperations/classification/areanumbering.h \
    rasteroperations/filter/rankorderrasterfilter.h \
    rasteroperations/filter/linearrasterfilter.h


OTHER_FILES += \ 
    rasteroperations/rasteroperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos

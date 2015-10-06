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
    rasteroperations/filter/linearrasterfilter.cpp \
    rasteroperations/mirrorrotateraster.cpp \
    rasteroperations/classification/clusterraster.cpp \
    rasteroperations/pointtoraster.cpp\
    rasteroperations/movingaverage.cpp\
    rasteroperations/polygontoraster.cpp\
    rasteroperations/filter/timesat_nrs.cpp


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
    rasteroperations/filter/linearrasterfilter.h \
    rasteroperations/mirrorrotateraster.h \
    rasteroperations/classification/clusterraster.h \
    rasteroperations/pointtoraster.h\
    rasteroperations/movingaverage.h\
    rasteroperations/polygontoraster.h\
    rasteroperations/filter/timesat_nrs.h


OTHER_FILES += \ 
    rasteroperations/rasteroperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET


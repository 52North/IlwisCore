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

DEFINES += PROJECTIONIMPLPROJ4_LIBRARY

SOURCES += \ 
    rasteroperations/rasteroperationsmodule.cpp \
    rasteroperations/aggregateraster.cpp


HEADERS += \
    rasteroperations/rasteroperationsmodule.h \
    rasteroperations/aggregateraster.h


OTHER_FILES += \ 
    rasteroperations/rasteroperations.json

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/projectionimplproj4 \
                $$PWD/../external/proj4
DEPENDPATH += $$PWD/projectionimplproj4

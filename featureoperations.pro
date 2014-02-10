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

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

DEFINES += FEATUREOPERATIONS_LIBRARY

HEADERS += \
    featureoperations/featureoperationsmodule.h \
    featureoperations/gridding.h

SOURCES += \
    featureoperations/featureoperationsmodule.cpp \
    featureoperations/gridding.cpp

OTHER_FILES += \ 
    featureoperations/featureoperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos


win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos

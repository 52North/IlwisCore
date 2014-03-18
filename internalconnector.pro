#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T21:32:12
#
#-------------------------------------------------

CONFIG += plugin
TARGET = internalconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       += sql
QT       -= gui

TEMPLATE = lib

DEFINES += InternalConnector_LIBRARY

HEADERS += \  
    internalconnector/internalmodule.h \
    internalconnector/internalilwisobjectfactory.h \
    internalconnector/internaltableconnector.h \
    internalconnector/internalprjmplfactory.h \
    internalconnector/projections/platecaree.h \
    internalconnector/projections/projectionimplementationinternal.h \
    internalconnector/epsg.h \
    internalconnector/internalfeaturecoverageconnector.h \
    internalconnector/internalrastercoverageconnector.h \
    internalconnector/internalcatalogexplorer.h

SOURCES += \
    internalconnector/internalmodule.cpp \
    internalconnector/internalilwisobjectfactory.cpp \
    internalconnector/internaltableconnector.cpp \
    internalconnector/internalprjimplfactory.cpp \
    internalconnector/projections/platecaree.cpp \
    internalconnector/projections/projectionimplementationinternal.cpp \
    internalconnector/internalfeaturecoverageconnector.cpp \
    internalconnector/internalrastercoverageconnector.cpp \
    internalconnector/internalcatalogexplorer.cpp

OTHER_FILES += \
    internalconnector/internalconnector.json


LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos

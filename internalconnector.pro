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
    internalconnector/internalcatalogconnector.h \
    internalconnector/internalgridcoverageconnector.h \
    internalconnector/internalilwisobjectfactory.h \
    internalconnector/internaltableconnector.h \
    internalconnector/internalprjmplfactory.h \
    internalconnector/projections/platecaree.h \
    internalconnector/projections/projectionimplementationinternal.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core

SOURCES += \  
    internalconnector/internalmodule.cpp \
    internalconnector/internalcatalogconnector.cpp \
    internalconnector/internalgridcoverageconnector.cpp \
    internalconnector/internalilwisobjectfactory.cpp \
    internalconnector/internaltableconnector.cpp \
    internalconnector/internalprjimplfactory.cpp \
    internalconnector/projections/platecaree.cpp \
    internalconnector/projections/projectionimplementationinternal.cpp

OTHER_FILES += \
    internalconnector/internalconnector.json


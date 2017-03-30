#-------------------------------------------------
#
# Project created by QtCreator 2012-04-22T21:32:12
#
#-------------------------------------------------

CONFIG += plugin
TARGET = internalconnector

include(global.pri)

QT       += sql

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
    internalconnector/internalcatalogexplorer.h \
    internalconnector/internalgeoreferenceconnector.h \
    internalconnector/internaldomain.h \
    internalconnector/internalcoordinatesystemconnector.h \
    internalconnector/internalrepresentation.h \
    internalconnector/workflowconnector.h \
    internalconnector/operationmetadataconnector.h \
    internalconnector/projections/cylindsinusinterrupt2.h \
    internalconnector/internalprojectionconnector.h \
    internalconnector/internalellipsoidconnector.h \
    internalconnector/internalscriptconnector.h \
    internalconnector/internalmodelconnector.h \
    internalconnector/internalcombinationmatrixconnector.h

SOURCES += \
    internalconnector/internalmodule.cpp \
    internalconnector/internalilwisobjectfactory.cpp \
    internalconnector/internaltableconnector.cpp \
    internalconnector/internalprjimplfactory.cpp \
    internalconnector/projections/platecaree.cpp \
    internalconnector/projections/projectionimplementationinternal.cpp \
    internalconnector/internalfeaturecoverageconnector.cpp \
    internalconnector/internalrastercoverageconnector.cpp \
    internalconnector/internalcatalogexplorer.cpp \
    internalconnector/internalgeoreferenceconnector.cpp \
    internalconnector/internaldomain.cpp \
    internalconnector/internalcoordinatesystemconnector.cpp \
    internalconnector/internalrepresentation.cpp \
    internalconnector/workflowconnector.cpp \
    internalconnector/operationmetadataconnector.cpp \
    internalconnector/projections/cylindsinusinterrupt2.cpp \
    internalconnector/internalprojectionconnector.cpp \
    internalconnector/internalellipsoidconnector.cpp \
    internalconnector/internalscriptconnector.cpp \
    internalconnector/internalmodelconnector.cpp \
    internalconnector/internalcombinationmatrixconnector.cpp

OTHER_FILES += \
    internalconnector/internalconnector.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}


linux{
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target
}

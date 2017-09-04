#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

CONFIG += plugin
TARGET = ilwis3connector

include(global.pri)

QT       += sql

TEMPLATE = lib

DEFINES += ILWIS3CONNECTOR_LIBRARY

SOURCES += \
    ilwis3connector/ilwis3connector.cpp \
    ilwis3connector/ilwis3module.cpp \
    ilwis3connector/ilwis3objectfactory.cpp \
    ilwis3connector/ellipsoidconnector.cpp \
    ilwis3connector/coordinatesystemconnector.cpp \
    ilwis3connector/domainconnector.cpp \
    ilwis3connector/georefconnector.cpp \
    ilwis3connector/coverageconnector.cpp \
    ilwis3connector/inifile.cpp \
    ilwis3connector/tableconnector.cpp \
    ilwis3connector/binaryilwis3table.cpp \
    ilwis3connector/ilwis3range.cpp \
    ilwis3connector/odfitem.cpp \
    ilwis3connector/ilwis3projectionconnector.cpp \
    ilwis3connector/RawConverter.cpp \
    ilwis3connector/ilwisrastercoverageconnector.cpp \
    ilwis3connector/ilwis3featureconnector.cpp \
    ilwis3connector/ilwis3catalogexplorer.cpp \
    ilwis3connector/ilwis3representationconnector.cpp \
    ilwis3connector/ilwis3catalogfileexplorer.cpp

HEADERS += \
    ilwis3connector/Ilwis3Connector_global.h \
    ilwis3connector/ilwis3connector.h \
    ilwis3connector/ilwis3module.h \
    ilwis3connector/ilwis3objectfactory.h \
    ilwis3connector/ellipsoidconnector.h \
    ilwis3connector/coordinatesystemconnector.h \
    ilwis3connector/domainconnector.h \
    ilwis3connector/georefconnector.h \
    ilwis3connector/coverageconnector.h \
    ilwis3connector/rawconverter.h \
    ilwis3connector/inifile.h \
    ilwis3connector/tableconnector.h \
    ilwis3connector/binaryilwis3table.h \
    ilwis3connector/ilwis3range.h \
    ilwis3connector/odfitem.h \
    ilwis3connector/ilwis3projectionconnector.h \
    ilwis3connector/ilwisrastercoverageconnector.h \
    ilwis3connector/ilwis3featureconnector.h \
    ilwis3connector/ilwis3catalogexplorer.h \
    ilwis3connector/ilwis3representationconnector.h \
    ilwis3connector/ilwis3catalogfileexplorer.h

resources.files += ilwis3connector/resources/projection_allias.csv \
    ilwis3connector/resources/ellipsoid_allias.csv \
    ilwis3connector/resources/datum_allias.csv \
    ilwis3connector/resources/domain_allias.csv \
    ilwis3connector/resources/ilwis3formats.config

resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources

OTHER_FILES += \
    ilwis3connector/ilwis3connector.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore


win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET


linux{
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target
}

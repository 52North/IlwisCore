##-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = streamconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT += network gui

TEMPLATE = lib

DEFINES += STREAMCONNECTOR_LIBRARY

OTHER_FILES += \
    streamconnector/streamconnector.json 

win32{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
}

linux{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$GEOSLIB/ -lgeos-3.4.2
}


win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos 

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \

HEADERS += \
    streamconnector/streamobjectfactory.h \
    streamconnector/streammodule.h \
    streamconnector/streamconnector.h \
    streamconnector/versioneddatastreamfactory.h \
    streamconnector/rawconverter.h \
    streamconnector/versionedserializer.h \
    streamconnector/tableserializerv1.h \
    streamconnector/rasterserializerv1.h \
    streamconnector/projectionserializerv1.h \
    streamconnector/georefserializerv1.h \
    streamconnector/featureserializerv1.h \
    streamconnector/ellipsoidserializerv1.h \
    streamconnector/domainserializerv1.h \
    streamconnector/coverageserializerv1.h \
    streamconnector/coordinatesystemserializerv1.h \
    streamconnector/downloadmanager.h \    
    streamconnector/remotecatalogexplorer.h \
    streamconnector/catalogserializerv1.h \
    streamconnector/catalogconnection.h \
    streamconnector/streamcatalogexplorer.h \
    streamconnector/operationmetadataserializerv1.h \
    streamconnector/workflowserializerv1.h \
    streamconnector/scriptserializerv1.h \
    streamconnector/modelserializerv1.h \
    streamconnector/combinationmatrixserializerv1.h

SOURCES += \
    streamconnector/streammodule.cpp \
    streamconnector/streamconnector.cpp \
    streamconnector/streamobjectfactory.cpp \
    streamconnector/rawconverter.cpp \
    streamconnector/versionedserializer.cpp \
    streamconnector/tableserializerv1.cpp \
    streamconnector/rasterserializerv1.cpp \
    streamconnector/projectionserializerv1.cpp \
    streamconnector/georefserializerv1.cpp \
    streamconnector/featureserializerv1.cpp \
    streamconnector/ellipsoidserializerv1.cpp \
    streamconnector/domainserializerv1.cpp \
    streamconnector/coverageserializerv1.cpp \
    streamconnector/coordinatesystemserializerv1.cpp \
    streamconnector/versioneddatastreamfactory.cpp \
    streamconnector/downloadmanager.cpp \
    streamconnector/remotecatalogexplorer.cpp \
    streamconnector/catalogserializerv1.cpp \
    streamconnector/catalogconnection.cpp \
    streamconnector/streamcatalogexplorer.cpp \
    streamconnector/operationmetadataserializerv1.cpp \
    streamconnector/workflowserializerv1.cpp \
    streamconnector/scriptserializerv1.cpp \
    streamconnector/modelserializerv1.cpp \
    streamconnector/combinationmatrixserializerv1.cpp

resources.files = streamconnector/resources/streamformats.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources


linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

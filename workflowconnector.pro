##-------------------------------------------------
#
# Project created by QtCreator 2016-02-01T08:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = workflowconnector

include(global.pri)

QT = network sql
QT       -= gui

TEMPLATE = lib

DEFINES += workflowconnector_LIBRARY

win32{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore 
}

linux{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore 
}

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/core\ilwisobjects\operation \
                $$PWD/core\ilwisobjects\workflow

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos 

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

OTHER_FILES += \
    workflowconnector/workflowconnector.json

HEADERS += \
    workflowconnector/workflowconnectormodule.h \ 
    workflowconnector/workflowcatalogexplorer.h \
    workflowconnector/workflowconnectorobjectfactory.h \
    workflowconnector/workflowjsonconnector.h \
    workflowconnector/jsonconfig.h

SOURCES += \
    workflowconnector/workflowconnectormodule.cpp \ 
    workflowconnector/workflowcatalogexplorer.cpp \
    workflowconnector/workflowconnectorobjectfactory.cpp \
    workflowconnector/workflowjsonconnector.cpp \
    workflowconnector/jsonconfig.cpp

linux{
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

DISTFILES += \
    workflowconnector/workflowconnector.json

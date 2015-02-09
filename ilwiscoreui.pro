#-------------------------------------------------
#
# Project created by QtCreator 2015-01-06T09:13:34
#
#-------------------------------------------------
include(global.pri)

QT       += qml quick opengl sql

TARGET = ilwiscoreui
TEMPLATE = lib

DEFINES += ILWISCOREUI_LIBRARY

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

INCLUDEPATH += $$PWD/../ilwiscore/core
DEPENDPATH += $$PWD/../ilwiscore/core

INCLUDEPATH +=  core/ilwisobjects/domain \
                ilwiscoreui \
                ilwiscoreui\models

HEADERS += ilwiscoreui/ilwiscoreui_global.h \
    ilwiscoreui/models/attributemodel.h \
    ilwiscoreui/models/catalogfiltermodel.h \
    ilwiscoreui/models/catalogmodel.h \
    ilwiscoreui/models/catalogscontroller.h \
    ilwiscoreui/models/domainitemmodel.h \
    ilwiscoreui/models/ilwisobjectmodel.h \
    ilwiscoreui/models/mastercatalogmodel.h \
    ilwiscoreui/models/operationcatalogmodel.h \
    ilwiscoreui/models/operationmodel.h \
    ilwiscoreui/models/operationsbykeymodel.h \
    ilwiscoreui/models/resourcemodel.h \
    ilwiscoreui/models/tranquilizerhandler.h \
    ilwiscoreui/models/uicontextmodel.h \
    ilwiscoreui/models/usermessagehandler.h \
    ilwiscoreui/models/visualizationmanager.h \
    ilwiscoreui/propertyeditors/representationsetter.h \
    ilwiscoreui/propertyeditors/propertyeditor.h \
    ilwiscoreui/models/coveragelayermodel.h \
    ilwiscoreui/drawers/drawerinterface.h \
    ilwiscoreui/propertyeditors/linestylesetter.h \
    ilwiscoreui/models/projectionparametermodel.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    ilwiscoreui/models/attributemodel.cpp \
    ilwiscoreui/models/catalogfiltermodel.cpp \
    ilwiscoreui/models/catalogmodel.cpp \
    ilwiscoreui/models/catalogscontroller.cpp \
    ilwiscoreui/models/domainitemmodel.cpp \
    ilwiscoreui/models/ilwisobjectmodel.cpp \
    ilwiscoreui/models/mastercatalogmodel.cpp \
    ilwiscoreui/models/operationcatalogmodel.cpp \
    ilwiscoreui/models/operationmodel.cpp \
    ilwiscoreui/models/operationsbykeymodel.cpp \
    ilwiscoreui/models/resourcemodel.cpp \
    ilwiscoreui/models/tranquilizerhandler.cpp \
    ilwiscoreui/models/uicontextmodel.cpp \
    ilwiscoreui/models/usermessagehandler.cpp \
    ilwiscoreui/models/visualizationmanager.cpp \
    ilwiscoreui/propertyeditors/representationsetter.cpp \
    ilwiscoreui/propertyeditors/propertyeditor.cpp \
    ilwiscoreui/models/coveragelayermodel.cpp \
    ilwiscoreui/drawers/drawerinterface.cpp \
    ilwiscoreui/propertyeditors/linestylesetter.cpp \
    ilwiscoreui/models/projectionparametermodel.cpp

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}

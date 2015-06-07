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
    ilwiscoreui/models/coveragelayermodel.h \
    ilwiscoreui/drawers/drawerinterface.h \
    ilwiscoreui/models/projectionparametermodel.h \
    ilwiscoreui/drawers/layersviewcommandinterface.h \
    ilwiscoreui/drawers/draweroperation.h \
    ilwiscoreui/drawers/drawerattributesetter.h \
    ilwiscoreui/drawers/drawerattributesetterfactory.h \
    ilwiscoreui/drawers/drawerfactory.h \
    ilwiscoreui/drawers/attributevisualproperties.h \
    ilwiscoreui/propertyeditors/attributesetter.h \
    ilwiscoreui/propertyeditors/layertransparencysetter.h \
    ilwiscoreui/propertyeditors/mapinformationattributesetter.h \
    ilwiscoreui/propertyeditors/polygonattributesetter.h \
    ilwiscoreui/propertyeditors/attributeeditor.h \
    ilwiscoreui/propertyeditors/lineattributesetter.h \
    ilwiscoreui/models/tablemodel.h \
    ilwiscoreui/models/workflow/workflowcatalogmodel.h \
    ilwiscoreui/models/workflow/workflowmodel.h \
    ilwiscoreui/models/visualattributemodel.h \
    ilwiscoreui/models/layerinfoitem.h \
    ilwiscoreui/models/layermanager.h \
    ilwiscoreui/models/catalogmapitem.h \
    ilwiscoreui/propertyeditors/numericrepresentationsetter.h \
    ilwiscoreui/drawers/rasterimage.h \
    ilwiscoreui/drawers/rasterimagefactory.h \
    ilwiscoreui/desktoptranquilizer.h \
    ilwiscoreui/models/columnmodel.h \
    ilwiscoreui/tableoperations/tableoperation.h \
    ilwiscoreui/tableoperations/tableoperationfactory.h \
    ilwiscoreui/tableoperations/sortcolumn.h \
    ilwiscoreui/models/workspacemodel.h \
    ilwiscoreui/models/graphmodel.h \
    ilwiscoreui/models/chartmodel.h \
    ilwiscoreui/models/consolescriptmodel.h
    ilwiscoreui/models/datapanemodel.h \
    ilwiscoreui/models/tabmodel.h

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
    ilwiscoreui/models/coveragelayermodel.cpp \
    ilwiscoreui/drawers/drawerinterface.cpp \
    ilwiscoreui/models/projectionparametermodel.cpp \
    ilwiscoreui/drawers/layersviewcommandinterface.cpp \
    ilwiscoreui/drawers/draweroperation.cpp \
    ilwiscoreui/drawers/drawerattributesetter.cpp \
    ilwiscoreui/drawers/drawerattributesetterfactory.cpp \
    ilwiscoreui/drawers/drawerfactory.cpp \
    ilwiscoreui/drawers/attributevisualproperties.cpp \
    ilwiscoreui/propertyeditors/attributesetter.cpp \
    ilwiscoreui/propertyeditors/layertransparencysetter.cpp \
    ilwiscoreui/propertyeditors/lineattributesetter.cpp \
    ilwiscoreui/propertyeditors/mapinformationattributesetter.cpp \
    ilwiscoreui/propertyeditors/polygonattributesetter.cpp \
    ilwiscoreui/propertyeditors/attributeeditor.cpp \
    ilwiscoreui/models/tablemodel.cpp \
    ilwiscoreui/models/workflow/workflowcatalogmodel.cpp \
    ilwiscoreui/models/workflow/workflowmodel.cpp \
    ilwiscoreui/models/visualattributemodel.cpp \
    ilwiscoreui/models/layerinfoitem.cpp \
    ilwiscoreui/models/layermanager.cpp \
    ilwiscoreui/models/catalogmapitem.cpp \
    ilwiscoreui/propertyeditors/numericrepresentationsetter.cpp \
    ilwiscoreui/drawers/rasterimage.cpp \
    ilwiscoreui/drawers/rasterimagefactory.cpp \
    ilwiscoreui/desktoptranquilizer.cpp \
    ilwiscoreui/models/columnmodel.cpp \
    ilwiscoreui/tableoperations/tableoperation.cpp \
    ilwiscoreui/tableoperations/tableoperationfactory.cpp \
    ilwiscoreui/tableoperations/sortcolumn.cpp \
    ilwiscoreui/models/workspacemodel.cpp \
    ilwiscoreui/models/graphmodel.cpp \
    ilwiscoreui/models/chartmodel.cpp \
    ilwiscoreui/models/consolescriptmodel.cpp
    ilwiscoreui/models/datapanemodel.cpp \

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}

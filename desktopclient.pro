QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations



CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
}

OUTDIR = $$PWD/..output/$$CONF
PLATFORM = generic
win32{
    PLATFORM = win32
}
TARGET = ilwisclient
DESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin

include(deployment.pri)

# Add more folders to ship with the application, here
folder_01.source = desktopclient/qml

win32{
    folder_01.target = ../output/$$PLATFORM$$CONF/bin
}

linux{
    folder_01.target = ../output/$$PLATFORM$$CONF/bin
}

DEPLOYMENTFOLDERS = folder_01


QT += qml quick sql opengl

BOOST=../external

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# The .cpp file which was generated for your project. Feel free to hack it.

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscoreui

win32{
    PLATFORM = win32
    BOOST=../external
    SHAREDEXT=dll
    PREFIXSHARED=
}
linux{
    BOOST=/usr/include
    GEOSINCL=/usr/include
    GEOSLIB=/usr/lib
    SHAREDEXT=so
    PREFIXSHARED=lib
    INCLUDEPATH += $$GEOSINCL
    DEPENDPATH += $$GEOSINCL
    LIBS += -L$$GEOSLIB/ -lgeos-3.4.2
}


INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core
# INCLUDEPATH += $$PWD/../external/geos
# DEPENDPATH += $$PWD/../external/geos
INCLUDEPATH += $$PWD/ilwiscoreui
DEPENDPATH += $$PWD/ilwiscoreui

INCLUDEPATH += core/ilwisobjects \
               core/ilwisobjects/geometry \
               core/util \
               core/ilwisobjects/geometry/geodeticdatum \
               core/ilwisobjects/geometry/projection \
               core/ilwisobjects/geometry/coordinatesystem \
               core/ilwisobjects/geometry/georeference \
               core/ilwisobjects/coverage \
               core/ilwisobjects/table \
               core/ilwisobjects/operation \
               core/ilwisobjects/operation/modeller \
               core/ilwisobjects/representation \
               core/catalog \
               core/geos/include \
               core/ilwisobjects/domain \
               ilwiscoreui \
               ilwiscoreui/models \
               ilwiscoreui/models/workflow \
                core \
                $$BOOST

OTHER_FILES += \
    desktopclient/qml/panel/PullOver.qml \
    desktopclient/qml/panel/CollapsiblePanel.qml \
    desktopclient/qml/DesktopClient.qml \
    desktopclient/qml/views/DummyList.qml \
    desktopclient/qml/panel/FolderSource.qml \
    desktopclient/qml/panel/Visualize.qml \
    desktopclient/qml/Visualization.qml \
    desktopclient/qml/panel/CatalogTable.qml \
    desktopclient/qml/panel/CatalogThumbGrid.qml \
    desktopclient/qml/Global.js \
    desktopclient/qml/workbench/propertyform/NumericDomainProperties.qml \
    desktopclient/qml/workbench/propertyform/GeoreferencePart.qml \
    desktopclient/qml/datapanel/visualization/RepresentationProperties.qml \
    desktopclient/qml/workbench/modeller/ModellerCatagoriesList.qml \
    desktopclient/qml/workbench/modeller/ModellerList.qml \
    desktopclient/qml/workbench/modeller/ModellerSelection.qml \
    desktopclient/qml/datapanel/workflow/AbstractBasicWorkflowDrawObjectqml \
    desktopclient/qml/datapanel/workflow/OperationWorkflowDrawObject.qml


# Installation path


# Please do not modify the following two lines. Required for deployment.
qtcAddDeployment()

HEADERS += \
    desktopclient/applicationformexpressionparser.h \
    desktopclient/ilwistype.h \
    desktopclient/keyfilter.h \
    desktopclient/workflowmetadataformbuilder.h \
    desktopclient/startilwis.h \
    core/buildnumber.h

SOURCES += \
    desktopclient/applicationformexpressionparser.cpp \
    desktopclient/ilwistype.cpp \
    desktopclient/main.cpp \
    desktopclient/keyfilter.cpp \
    desktopclient/workflowmetadataformbuilder.cpp \
    desktopclient/startilwis.cpp

DISTFILES += \
    desktopclient/qml/datapanel/visualization/Layers.qml \
    desktopclient/qml/datapanel/visualization/PropertyEditorsList.qml \
    desktopclient/qml/datapanel/visualization/propertyeditors/LayersList.qml \
    desktopclient/qml/datapanel/visualization/propertyeditors/VisualAttributeList.qml \
    desktopclient/qml/controls/HScrollBar.qml \
    desktopclient/qml/workbench/WorkSpaceDataGrid.qml \
    desktopclient/qml/datapanel/visualization/MapPanel.qml \
    desktopclient/qml/datapanel/visualization/MapPanelButton.qml \
    desktopclient/qml/datapanel/visualization/MapPanelButtonBar.qml \
    desktopclient/qml/datapanel/ButtonBarTabViewStyle.qml \
    desktopclient/qml/datapanel/visualization/propertyeditors/ConvertAttributeDomain.qml \
    desktopclient/qml/datapanel/visualization/propertyeditors/LayerOpacity.qml \
    desktopclient/qml/datapanel/workflow/AttachmentRectangle.qml \
    desktopclient/qml/workbench/creators/PaletteColorList.qml \
    desktopclient/qml/workbench/creators/CreateGeorefCorners.qml \
    desktopclient/qml/workbench/preferences/Locations.qml \
    desktopclient/qml/datapanel/catalog/CatalogActions.qml \
    desktopclient/qml/controls/ActionButtonV.qml


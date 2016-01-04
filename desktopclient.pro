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
    folder_01.target = ../../../output/$$PLATFORM$$CONF/bin
}

linux{
    folder_01.target = ../output/$$PLATFORM$$CONF/bin
}

DEPLOYMENTFOLDERS = folder_01


QT += qml quick sql

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
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
    INCLUDEPATH += $$PWD/../external/geos
    DEPENDPATH += $$PWD/../external/geos
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


INCLUDEPATH += $$PWD/../IlwisCore/core
DEPENDPATH += $$PWD/../IlwisCore/core
INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos
INCLUDEPATH += $$PWD/../IlwisCore/ilwiscoreui
DEPENDPATH += $$PWD/../IlwisCore/ilwiscoreui

INCLUDEPATH +=  ../IlwisCore/core/ilwisobjects \
                ../IlwisCore/core/ilwisobjects/geometry \
                ../IlwisCore/core/util \
                ../IlwisCore/core/ilwisobjects/geometry/geodeticdatum \
                ../IlwisCore/core/ilwisobjects/geometry/projection \
                ../IlwisCore/core/ilwisobjects/geometry/coordinatesystem \
                ../IlwisCore/core/ilwisobjects/geometry/georeference \
                ../IlwisCore/core/ilwisobjects/coverage \
                ../IlwisCore/core/ilwisobjects/table \
                ../IlwisCore/core/ilwisobjects/operation \
                ../IlwisCore/core/ilwisobjects/workflow \
                ../IlwisCore/core/ilwisobjects/representation \
                ../IlwisCore/core/catalog \
                ../IlwisCore/core/ilwisobjects/domain \
                ../IlwisCore/ilwiscoreui \
                ../IlwisCore/ilwiscoreui/models \
                ../IlwisCore/ilwiscoreui/models/workflow \
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
    desktopclient/workflowmetadataformbuilder.h

SOURCES += \
    desktopclient/applicationformexpressionparser.cpp \
    desktopclient/ilwistype.cpp \
    desktopclient/main.cpp \
    desktopclient/keyfilter.cpp \
    desktopclient/workflowmetadataformbuilder.cpp

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
    desktopclient/qml/workbench/preferences/Locations.qml


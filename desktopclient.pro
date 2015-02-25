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
folder_01.target = ../../../output/$$PLATFORM$$CONF/bin
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
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscoreui

INCLUDEPATH += $$PWD/../ilwiscore/core
DEPENDPATH += $$PWD/../ilwiscore/core
INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos
INCLUDEPATH += $$PWD/../ilwiscore/ilwiscoreui
DEPENDPATH += $$PWD/../ilwiscore/ilwiscoreui

INCLUDEPATH +=  ../ilwiscore/core/ilwisobjects \
                ../ilwiscore/core/ilwisobjects/geometry \
                ../ilwiscore/core/util \
                ../ilwiscore/core/ilwisobjects/geometry/geodeticdatum \
                ../ilwiscore/core/ilwisobjects/geometry/projection \
                ../ilwiscore/core/ilwisobjects/geometry/coordinatesystem \
                ../ilwiscore/core/ilwisobjects/geometry/georeference \
                ../ilwiscore/core/ilwisobjects/coverage \
                ../ilwiscore/core/ilwisobjects/table \
                ../ilwiscore/core/ilwisobjects/operation \
                ../ilwiscore/core/ilwisobjects/representation \
                ../ilwiscore/core/catalog \
                ../ilwiscore/core/ilwisobjects/domain \
                ../ilwiscore/ilwiscoreui \
                ../ilwiscore/ilwiscoreui/models \
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
    desktopclient/qml/datapanel/modeller/ConnectorObject.qml \
    desktopclient/qml/datapanel/modeller/DataSourceObject.qml \
    desktopclient/qml/datapanel/modeller/OperationObject.qml


# Installation path


# Please do not modify the following two lines. Required for deployment.
qtcAddDeployment()

HEADERS += \
    desktopclient/applicationformexpressionparser.h \
    desktopclient/ilwistype.h \
    desktopclient/keyfilter.h

SOURCES += \
    desktopclient/applicationformexpressionparser.cpp \
    desktopclient/ilwistype.cpp \
    desktopclient/main.cpp \
    desktopclient/keyfilter.cpp

DISTFILES += \
    desktopclient/qml/datapanel/visualization/Layers.qml




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
SOURCES += desktopclient/main.cpp \
    desktopclient/ilwistype.cpp \
    desktopclient/models/resourcemodel.cpp \
    desktopclient/models/catalogmodel.cpp \
    desktopclient/models/mastercatalogmodel.cpp \
    desktopclient/models/operationcatalogmodel.cpp \
    desktopclient/models/operationmodel.cpp \
    desktopclient/applicationformexpressionparser.cpp \
    desktopclient/models/usermessagehandler.cpp \
    desktopclient/models/tranquilizerhandler.cpp \
    desktopclient/models/objectvisualizationmodel.cpp \
    desktopclient/models/visualizationmanager.cpp \
    desktopclient/models/ilwisobjectmodel.cpp \
    desktopclient/models/attributemodel.cpp \
    desktopclient/models/domainitemmodel.cpp

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos

INCLUDEPATH += $$PWD/../ilwiscore/core
DEPENDPATH += $$PWD/../ilwiscore/core
INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos

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
                ../ilwiscore/core/catalog \
                ../ilwiscore/core/ilwisobjects/domain \
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
    desktopclient/qml/global.js \
    desktopclient/qml/workbench/propertyform/NumericDomainProperties.qml

HEADERS += \
    desktopclient/ilwistype.h \
    desktopclient/models/resourcemodel.h \
    desktopclient/models/catalogmodel.h \
    desktopclient/models/mastercatalogmodel.h \
    desktopclient/models/operationcatalogmodel.h \
    desktopclient/models/operationmodel.h \
    desktopclient/applicationformexpressionparser.h \
    desktopclient/models/usermessagehandler.h \
    desktopclient/models/tranquilizerhandler.h \
    desktopclient/models/objectvisualizationmodel.h \
    desktopclient/models/visualizationmanager.h \
    desktopclient/models/ilwisobjectmodel.h \
    desktopclient/models/attributemodel.h \
    desktopclient/models/domainitemmodel.h

# Installation path


# Please do not modify the following two lines. Required for deployment.
qtcAddDeployment()




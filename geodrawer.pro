QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

TEMPLATE = lib
TARGET = layersview
QT += qml quick sql opengl
CONFIG += qt plugin

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

TARGET = $$qtLibraryTarget($$TARGET)

# Input
SOURCES += \
    geodrawer/geodrawer_plugin.cpp \
    geodrawer/rootdrawer.cpp \
    geodrawer/simpledrawer.cpp \
    geodrawer/complexdrawer.cpp \
    geodrawer/basedrawer.cpp \
    geodrawer/spatialdatadrawer.cpp \
    geodrawer/layerdrawer.cpp \
    geodrawer/drawingcolor.cpp \
    geodrawer/featurelayerdrawer.cpp \
    geodrawer/linedrawer.cpp \
    geodrawer/openglhelper.cpp \
    geodrawer/tesselation/bucketalloc.c \
    geodrawer/tesselation/dict.c \
    geodrawer/tesselation/geom.c \
    geodrawer/tesselation/mesh.c \
    geodrawer/tesselation/priorityq.c \
    geodrawer/tesselation/sweep.c \
    geodrawer/tesselation/tess.c \
    geodrawer/tesselation/ilwistesselator.cpp \
    geodrawer/selectiondrawer.cpp \
    geodrawer/layersview.cpp \
    geodrawer/layersrenderer.cpp \
    geodrawer/operations/adddrawer.cpp \
    geodrawer/operations/setviewextents.cpp \
    geodrawer/operations/removedrawer.cpp \
    geodrawer/drawerattributesetters/simplelinesetter.cpp \
    geodrawer/drawerattributesetters/basespatialattributesetter.cpp \
    geodrawer/drawerattributesetters/simplepolygonsetter.cpp \
    geodrawer/drawerattributesetters/simplepointsetter.cpp \
    geodrawer/operations/setlayertransparency.cpp \
    geodrawer/operations/setlayervisibility.cpp \
    geodrawer/rasterlayerdrawer.cpp \
    geodrawer/operations/showpolygonboundaries.cpp \
    geodrawer/operations/showpolygonareas.cpp \
    geodrawer/operations/setpolygonboundarycolor.cpp \
    geodrawer/rasterimage.cpp \
    geodrawer/operations/setlinecolor.cpp \
    geodrawer/operations/setpolygonboundarywidth.cpp

HEADERS += \
    geodrawer/geodrawer_plugin.h \
    geodrawer/rootdrawer.h \
    geodrawer/simpledrawer.h \
    geodrawer/complexdrawer.h \
    geodrawer/basedrawer.h \
    geodrawer/spatialdatadrawer.h \
    geodrawer/layerdrawer.h \
    geodrawer/drawingcolor.h \
    geodrawer/featurelayerdrawer.h \
    geodrawer/linedrawer.h \
    geodrawer/openglhelper.h \
    geodrawer/tesselation/bucketalloc.h \
    geodrawer/tesselation/dict.h \
    geodrawer/tesselation/geom.h \
    geodrawer/tesselation/glfw3.h \
    geodrawer/tesselation/mesh.h \
    geodrawer/tesselation/priorityq.h \
    geodrawer/tesselation/sweep.h \
    geodrawer/tesselation/tess.h \
    geodrawer/tesselation/tesselator.h \
    geodrawer/tesselation/ilwistesselator.h \
    geodrawer/selectiondrawer.h \
    geodrawer/layersview.h \
    geodrawer/layersrenderer.h \
    geodrawer/operations/adddrawer.h \
    geodrawer/operations/setviewextents.h \
    geodrawer/operations/removedrawer.h \
    geodrawer/drawerattributesetters/simplelinesetter.h \
    geodrawer/drawerattributesetters/basespatialattributesetter.h \
    geodrawer/drawerattributesetters/simplepolygonsetter.h \
    geodrawer/drawerattributesetters/simplepointsetter.h \
    geodrawer/operations/setlayertransparency.h \
    geodrawer/operations/setlayervisibility.h \
    geodrawer/rasterlayerdrawer.h \
    geodrawer/operations/showpolygonboundaries.h \
    geodrawer/operations/showpolygonareas.h \
    geodrawer/operations/setpolygonboundarycolor.h \
    geodrawer/rasterimage.h \
    geodrawer/operations/setlinecolor.h \
    geodrawer/operations/setpolygonboundarywidth.h


OTHER_FILES = geodrawer/qmldir

BOOST=../external
ILWISCORE = ../IlwisCore
INCLUDEPATH += $$PWD/$$ILWISCORE/core
DEPENDPATH += $$PWD/$$ILWISCORE/core
INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos
INCLUDEPATH += $$PWD/../ilwiscore/ilwiscoreui
DEPENDPATH += $$PWD/../ilwiscore/ilwiscoreui

INCLUDEPATH +=  core/ilwisobjects/domain \
                ilwiscoreui \
                ilwiscoreui\models

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscoreui


INCLUDEPATH +=  $$ILWISCORE/core/ilwisobjects \
                $$ILWISCORE/core/ilwisobjects/geometry \
                $$ILWISCORE/core/util \
                $$ILWISCORE/core/ilwisobjects/geometry/geodeticdatum \
                $$ILWISCORE/core/ilwisobjects/geometry/projection \
                $$ILWISCORE/core/ilwisobjects/geometry/coordinatesystem \
                $$ILWISCORE/core/ilwisobjects/geometry/georeference \
                $$ILWISCORE/core/ilwisobjects/coverage \
                $$ILWISCORE/core/ilwisobjects/table \
                $$ILWISCORE/core/ilwisobjects/operation \
                $$ILWISCORE/core/ilwisobjects/representation \
                $$ILWISCORE/core/catalog \
                $$ILWISCORE/core/ilwisobjects/domain \
                $$ILWISCORE \
                $$BOOST

DESTDIR = $$PWD/../libraries/win32debug/extensions/ui/LayersView
DLLDESTDIR = $$PWD/../output/win32debug/bin/extensions/ui/LayersView

!equals(_PRO_FILE_PWD_, $$DLLDESTDIR) {
    copy_qmldir.target = $$DLLDESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/geodrawer/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = geodrawer/qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}


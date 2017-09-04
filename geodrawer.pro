QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
# QMAKE_CXXFLAGS += -fsanitize=address
#QMAKE_CXXFLAGS += -fstack-protector-all


TEMPLATE = lib
TARGET = LayersView
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

# Input
SOURCES += \
    geodrawer/geodrawer_plugin.cpp \
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
    geodrawer/layersview.cpp \
    geodrawer/layersrenderer.cpp \
    geodrawer/operations/adddrawer.cpp \
    geodrawer/operations/setviewextents.cpp \
    geodrawer/operations/removedrawer.cpp \
    geodrawer/drawerattributesetters/simplelinesetter.cpp \
    geodrawer/drawerattributesetters/basespatialattributesetter.cpp \
    geodrawer/drawerattributesetters/simplepolygonsetter.cpp \
    geodrawer/drawerattributesetters/simplepointsetter.cpp \
    geodrawer/operations/setlayervisibility.cpp \
    geodrawer/rasterlayerdrawer.cpp \
    geodrawer/operations/showpolygonboundaries.cpp \
    geodrawer/operations/showpolygonareas.cpp \
    geodrawer/operations/setpolygonboundarycolor.cpp \
    geodrawer/operations/setlinecolor.cpp \
    geodrawer/operations/setpolygonboundarywidth.cpp \
    geodrawer/rastervalueimage.cpp \
    geodrawer/rastercolorimage.cpp \
    geodrawer/operations/layermanagement.cpp \
    geodrawer/operations/representationattributefillcolor.cpp \
    geodrawer/operations/setrepresentation.cpp \
    geodrawer/operations/setbackgroundcolor.cpp \
    geodrawer/operations/setgridproperties.cpp \
    geodrawer/operations/setlayeropacity.cpp \
    geodrawer/rasteritemimage.cpp \
    geodrawer/operations/cleardrawers.cpp \
    geodrawer/operations/showcoverage.cpp \
    geodrawer/drawercolorsets.cpp

HEADERS += \
    geodrawer/geodrawer_plugin.h \
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
    geodrawer/layersview.h \
    geodrawer/layersrenderer.h \
    geodrawer/operations/adddrawer.h \
    geodrawer/operations/setviewextents.h \
    geodrawer/operations/removedrawer.h \
    geodrawer/drawerattributesetters/simplelinesetter.h \
    geodrawer/drawerattributesetters/basespatialattributesetter.h \
    geodrawer/drawerattributesetters/simplepolygonsetter.h \
    geodrawer/drawerattributesetters/simplepointsetter.h \
    geodrawer/operations/setlayervisibility.h \
    geodrawer/rasterlayerdrawer.h \
    geodrawer/operations/showpolygonboundaries.h \
    geodrawer/operations/showpolygonareas.h \
    geodrawer/operations/setpolygonboundarycolor.h \
    geodrawer/operations/setlinecolor.h \
    geodrawer/operations/setpolygonboundarywidth.h \
    geodrawer/rastervalueimage.h \
    geodrawer/rastercolorimage.h \
    geodrawer/operations/layermanagement.h \
    geodrawer/operations/representationattributefillcolor.h \
    geodrawer/operations/setrepresentation.h \
    geodrawer/operations/setbackgroundcolor.h \
    geodrawer/operations/setgridproperties.h \
    geodrawer/operations/setlayeropacity.h \
    geodrawer/rasteritemimage.h \
    geodrawer/operations/cleardrawers.h \
    geodrawer/operations/showcoverage.h \
    geodrawer/drawercolorsets.h


OTHER_FILES = geodrawer/qmldir

# BOOST=../external
INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core
# INCLUDEPATH += $$PWD/../external/geos
# DEPENDPATH += $$PWD/../external/geos
INCLUDEPATH += $$PWD/ilwiscoreui
DEPENDPATH += $$PWD/ilwiscoreui

INCLUDEPATH +=  core/ilwisobjects/domain \
                ilwiscoreui \
                ilwiscoreui\models \
                ilwiscoreui\drawers

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
    GEOSINCL=/usr/local/include
    GEOSLIB=/usr/local/lib
    SHAREDEXT=so
    PREFIXSHARED=lib
    INCLUDEPATH += $$GEOSINCL
    DEPENDPATH += $$GEOSINCL
    LIBS += -L$$GEOSLIB/ -lgeos-3.4.2
}


INCLUDEPATH +=  core/ilwisobjects \
                core/ilwisobjects/geometry \
                core/util \
                core/geos/include \
                core/ilwisobjects/geometry/geodeticdatum \
                core/ilwisobjects/geometry/projection \
                core/ilwisobjects/geometry/coordinatesystem \
                core/ilwisobjects/geometry/georeference \
                core/ilwisobjects/coverage \
                core/ilwisobjects/table \
                core/ilwisobjects/operation \
                core/ilwisobjects/representation \
                core/catalog \
                core/ilwisobjects/domain \
                ilwiscoreui/models \
                ilwiscoreui/drawers \
                $$BOOST
win32{
    DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/ui/LayersView
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/LayersView
}

linux{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET
}

!equals(_PRO_FILE_PWD_, $$DLLDESTDIR) {
    copy_qmldir.target = $$DLLDESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/geodrawer/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = geodrawer/qmldir
#unix {
#    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
#    qmldir.path = $$installPath
#    target.path = $$installPath
#    INSTALLS += target qmldir
#}

linux{
    DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/ui/$$TARGET
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET
    qmldir.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET

    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET

    INSTALLS += qmldir target dependencies
}

DISTFILES += \
    geodrawer/selectiondrawer.js

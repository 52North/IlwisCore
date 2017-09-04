QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
# QMAKE_CXXFLAGS += -fsanitize=address
#QMAKE_CXXFLAGS += -fstack-protector-all


TEMPLATE = lib
TARGET = SMCE
QT += qml quick sql opengl
CONFIG += qt plugin

CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
}
OUTDIR = $$PWD/../output/$$CONF
PLATFORM = generic
win32{
    PLATFORM = win32
}

# Input
SOURCES += \
    smce/smce_plugin.cpp \
    smce/smce.cpp \
    smce/smcemodel.cpp \
    smce/smcespatialalternatives.cpp \
    smce/modelsmcespatialalternatives.cpp \
    smce/node.cpp
 

HEADERS += \
    smce/smce_plugin.h \
    smce/smce.h \
    smce/smcemodel.h \
    smce/smcespatialalternatives.h \
    smce/modelsmcespatialalternatives.h \
    smce/node.h
  

OTHER_FILES = smce/qmldir

# BOOST=../external
INCLUDEPATH += $$PWD/core
DEPENDPATH += $$PWD/core
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

INCLUDEPATH +=  core/ilwisobjects \
               core/ilwisobjects/geometry \
               core/util \
               core/geos/include \
               core/ilwisobjects/operation/modeller \
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
    DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/ui/SMCE
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/SMCE
}

linux{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET
}

!equals(_PRO_FILE_PWD_, $$DLLDESTDIR) {
    copy_qmldir.target = $$DLLDESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/smce/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = SMCE/qmldir
qmldir.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET

qml.files = SMCE/*.qml
qml.files += SMCE/table
qml.files += SMCE/tree
qml.files += SMCE/controls
qml.path =  $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/ui/$$TARGET
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

    INSTALLS += qmldir qml target dependencies
}

INSTALLS += qml qmldir

DISTFILES += \
    smce/SMCEPanel.qml \
    smce/SMCEToolBar.qml \
    smce/SMCESpatialAlternativePanel.qml \
    smce/SMCEForm.qml \
    smce/SMCEAppForm.qml \
    smce/SMCEClassStandardization.qml \
    smce/SMCEClassStandardizationTable.qml \
    smce/table/AggregateCell.qml \
    smce/table/ColumnHeader.qml \
    smce/table/ColumnList.qml \
    smce/table/ColumnMetadataList.qml \
    smce/table/ColumnOperation.qml \
    smce/table/ColumnOperationList.qml \
    smce/table/ConvertColumn.qml \
    smce/table/FirstColumn.qml \
    smce/table/NormalCell.qml \
    smce/table/SortColumn.qml \
    smce/table/TableColumn.qml \
    smce/tree/Tree.qml \
    smce/controls/LabeledCheckBox.qml \
    smce/controls/DragPoint.qml \
    smce/SMCENodeEval.qml \
    smce/SMCEDefForm.qml \
    smce/SMCEEvalForm.qml \
    smce/SMCEGraphStandardization.qml \
    smce/SMCEValueConstraintStandardization.qml



QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

TEMPLATE = lib
TARGET = propertyeditors
QT += qml quick
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
    propertyeditors\propertyeditors_plugin.cpp \
    propertyeditors\representationsetter.cpp

HEADERS += \
    propertyeditors\propertyeditors_plugin.h \
    propertyeditors\representationsetter.h

OTHER_FILES = propertyeditors\qmldir \
    propertyeditors/RepresentationSetter.qml

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

DESTDIR = $$PWD/../libraries/win32debug/extensions/ui/PropertyEditors
DLLDESTDIR = $$PWD/../output/win32debug/bin/extensions/ui/PropertyEditors

!equals(_PRO_FILE_PWD_, $$DLLDESTDIR) {
    copy_qmldir.target = $$DLLDESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/propertyeditors/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
}



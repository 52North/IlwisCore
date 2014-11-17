QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

TEMPLATE = lib
TARGET = geodrawer
QT += qml quick
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = n52.org.ilwisobjects.GeoDrawer

# Input
SOURCES += \
    geodrawer_plugin.cpp \
    geodrawer.cpp

HEADERS += \
    geodrawer_plugin.h \
    geodrawer.h

OTHER_FILES = qmldir

DESTDIR = $$PWD/../../libraries/win32debug/extensions/ui/GeoDrawer
DLLDESTDIR = $$PWD/../../output/win32debug/bin/extensions/ui/GeoDrawer

!equals(_PRO_FILE_PWD_, $$DLLDESTDIR) {
    copy_qmldir.target = $$DLLDESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}


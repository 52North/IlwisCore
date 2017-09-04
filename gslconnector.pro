#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = gslconnector

include(global.pri)

TEMPLATE = lib

DEFINES += GSLCONNECTOR_LIBRARY

HEADERS += \
    gslconnector/relativeaggregaterasterstatistics.h \
    gslconnector/rastercorrelation.h \
    gslconnector/gslmodule.h \
    gslconnector/aggregaterasterstatistics.h \
    gslconnector/rastercovariance.h \
    gslconnector/rasterquantile.h

SOURCES += \
    gslconnector/relativeaggregaterasterstatistics.cpp \
    gslconnector/rastercorrelation.cpp \
    gslconnector/gslmodule.cpp \
    gslconnector/aggregaterasterstatistics.cpp \
    gslconnector/rastercovariance.cpp \
    gslconnector/rasterquantile.cpp

OTHER_FILES += \
    gslconnector/gslconnector.json \
    gslconnector/resources/libraries.config

win32 {
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/gslconnector -lgsl.dll

    INCLUDEPATH += $$PWD/../external/gsl
}

linux {
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L/usr/lib -lgsl

    INCLUDEPATH += /usr/include/gsl
}

SOURCE_RESOURCE = $$clean_path($$PWD/$$TARGET/resources/libraries.config)
TARGET_RESOURCE_DIR = $$clean_path($$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources/)
TARGET_RESOURCE = $$TARGET_RESOURCE_DIR/libraries.config
resources.target = $$TARGET_RESOURCE
linux {
    resources.commands = $$quote(test -d $$TARGET_RESOURCE_DIR || mkdir -p $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
}
win32 {
    SOURCE_RESOURCE = $$replace(SOURCE_RESOURCE,/,\\)
    TARGET_RESOURCE = $$replace(TARGET_RESOURCE,/,\\)
    TARGET_RESOURCE_DIR = $$replace(TARGET_RESOURCE_DIR,/,\\)
    resources.commands = $$quote(if not exist $$TARGET_RESOURCE_DIR mkdir $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
}
resources.commands += $$quote($(COPY) $$SOURCE_RESOURCE $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
resources.depends = $$SOURCE_RESOURCE
PRE_TARGETDEPS += $$TARGET_RESOURCE
QMAKE_EXTRA_TARGETS += resources

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

linux{
    dependencies.files = /usr/lib/libgslcblas.so \
        /usr/lib/libgsl.so

    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}



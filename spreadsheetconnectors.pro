CONFIG += plugin
TARGET = spreadsheetconnectors

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += SPREADSHEETCONNECTOR_LIBRARY

SOURCE_RESOURCE = $$clean_path($$PWD/$$TARGET/resources/libraries.config)
SOURCE_RESOURCE_2 = $$clean_path($$PWD/$$TARGET/resources/spreadsheetformats.config)
TARGET_RESOURCE_DIR = $$clean_path($$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources/)
TARGET_RESOURCE = $$TARGET_RESOURCE_DIR/libraries.config
TARGET_RESOURCE_2 = $$TARGET_RESOURCE_DIR/spreadsheetformats.config
resources.target = $$TARGET_RESOURCE
resources_2.target = $$TARGET_RESOURCE_2
linux {
    resources.commands = $$quote(test -d $$TARGET_RESOURCE_DIR || mkdir -p $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
    resources_2.commands = $$quote(test -d $$TARGET_RESOURCE_DIR || mkdir -p $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
}
win32 {
    SOURCE_RESOURCE = $$replace(SOURCE_RESOURCE,/,\\)
    TARGET_RESOURCE = $$replace(TARGET_RESOURCE,/,\\)
    SOURCE_RESOURCE_2 = $$replace(SOURCE_RESOURCE_2,/,\\)
    TARGET_RESOURCE_2 = $$replace(TARGET_RESOURCE_2,/,\\)
    TARGET_RESOURCE_DIR = $$replace(TARGET_RESOURCE_DIR,/,\\)
    resources.commands = $$quote(if not exist $$TARGET_RESOURCE_DIR mkdir $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
    resources_2.commands = $$quote(if not exist $$TARGET_RESOURCE_DIR mkdir $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
}
resources.commands += $$quote($(COPY) $$SOURCE_RESOURCE $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
resources_2.commands += $$quote($(COPY) $$SOURCE_RESOURCE_2 $$TARGET_RESOURCE_DIR$$escape_expand(\n\t))
resources.depends = $$SOURCE_RESOURCE
resources_2.depends = $$SOURCE_RESOURCE_2
PRE_TARGETDEPS += $$TARGET_RESOURCE $$TARGET_RESOURCE_2
QMAKE_EXTRA_TARGETS += resources resources_2

OTHER_FILES += \
    spreadsheetconnectors/spreadsheetconnectors.json \
    spreadsheetconnectors/resources/libraries.config \
    spreadsheetconnectors/resources/spreadsheetformats.config

win32{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lfreexl \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lQOds \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/spreadsheetconnectors -lQt0Xlsx
}


linux{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$GEOSLIB/ -lgeos-3.4.2 \
        -L$$LINUXLIB -lfreexl \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/QOds -lQOds \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/Qt0Xlsx -lQt0Xlsx
}


		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

win32{
INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/spreadsheet \
                $$PWD/../external/xlsx

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/ods \
                $$PWD/../external/xlsx
}

linux{
INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/QOds \
                $$PWD/../external/xlsx

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/QOds \
                $$PWD/../external/xlsx
}

HEADERS += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.h \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.h \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.h \
    spreadsheetconnectors/SpreadSheet.h \
    spreadsheetconnectors/odsformat.h \
    spreadsheetconnectors/spreadsheettableconnector.h \
    spreadsheetconnectors/xlsformat.h \
    spreadsheetconnectors/spreadsheetcatalogexplorer.h \
    spreadsheetconnectors/xlsxformat.h

SOURCES += \
    spreadsheetconnectors/spreadsheetconnectorsmodule.cpp \
    spreadsheetconnectors/spreadsheetconnectorsobjectfactory.cpp \
    spreadsheetconnectors/spreadsheetcatalogfileexplorer.cpp \
    spreadsheetconnectors/odsformat.cpp \
    spreadsheetconnectors/spreadsheettableconnector.cpp \
    spreadsheetconnectors/xlsformat.cpp \
    spreadsheetconnectors/spreadsheetcatalogexplorer.cpp \
    spreadsheetconnectors/xlsxformat.cpp

linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so \
        $$LINUXLIB\libquazip.so \
        $$LINUXLIB\libfreexl.so.1 \
        $$PWD/../libraries/$$PLATFORM$$CONF/QOds/libQOds.so \
        $$PWD/../libraries/$$PLATFORM$$CONF/Qt0Xlsx/libQt0Xlsx.so


    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

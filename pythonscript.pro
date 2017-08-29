# Instructions:
#
# First build project "pythonapi" (follow the instructions in pythonapi.pro if not yet done)
# Then build project "pythonscript"

CONFIG += plugin
TARGET = pythonscript

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += PYTHONSCRIPT_LIBRARY

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

OTHER_FILES += \
    pythonscript/pythonscript.json \
    pythonscript/resources/libraries.config

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/pythonapi -lpython35 \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/pythonapi -l_ilwisobjects
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/python35/include \
                $$PWD/../IlwisConnectors/pythonapi
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/python35/include

HEADERS += \
    pythonscript/pythonscriptmodule.h \
    pythonscript/runpython.h \
    pythonscript/pythonobjectfactory.h \
    pythonscript/pythonworkflowconnector.h \
    pythonscript/pythonscriptconnector.h \
    pythonscript/pythonscriptcatalogexplorer.h \
    pythonscript/callpythonoperation.h \
    pythonscript/generatescriptnode.h \
    pythonscript/runpythonprogram.h

SOURCES += \
    pythonscript/pythonscriptmodule.cpp \
    pythonscript/runpython.cpp \
    pythonscript/pythonobjectfactory.cpp \
    pythonscript/pythonworkflowconnector.cpp \
    pythonscript/pythonscriptconnector.cpp \
    pythonscript/pythonscriptcatalogexplorer.cpp \
    pythonscript/callpythonoperation.cpp \
    pythonscript/generatescriptnode.cpp \
    pythonscript/runpythonprogram.cpp





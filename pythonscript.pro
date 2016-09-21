CONFIG += plugin
TARGET = pythonscript

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += PYTHONSCRIPT_LIBRARY

resources.files += pythonscript/resources/libraries.config
resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources

INSTALLS += resources

OTHER_FILES += \
    spreadsheetconnectors/pythonscript.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET -lpython35
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/python35/include
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/python35/include

HEADERS += \
    pythonscript/pythonscriptmodule.h \
    pythonscript/runpython.h \
    pythonscript/pythonobjectfactory.h \
    pythonscript/pythonworkflowconnector.h

SOURCES += \
    pythonscript/pythonscriptmodule.cpp \
    pythonscript/runpython.cpp \
    pythonscript/pythonobjectfactory.cpp \
    pythonscript/pythonworkflowconnector.cpp





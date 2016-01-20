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
        -L$$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET -lC:/python35/libs/python35
		
win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/python 
DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/python 

HEADERS += \
    pythonscript/pythonscriptmodule.h \
    pythonscript/runpython.h

SOURCES += \
    pythonscript/pythonscriptmodule.cpp \
    pythonscript/runpython.cpp





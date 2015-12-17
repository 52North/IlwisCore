#-------------------------------------------------
#
# Project created by QtCreator 2012-06-14T18:29:31
#
#-------------------------------------------------

CONFIG += plugin
TARGET = ilwisscript

QMAKE_CXXFLAGS += -Wno-unknown-pragmas

include(global.pri)

QT       -= gui
QT += sql

TEMPLATE = lib

win32{
   ANTLRLIB=$PWD/../libraries/$$PLATFORM$$CONF/
}
linux {
    ANTLRLIB=/usr/local/lib
}

DEFINES += ILWISSCRIPT_LIBRARY

SOURCES += \
    ilwisscript/ilwisscriptmodule.cpp \
    ilwisscript/script.cpp \
    ilwisscript/ast/astnode.cpp \
    ilwisscript/parserlexer/IlwisScriptLexer.cpp \
    ilwisscript/parserlexer/IlwisScriptParser.cpp \
    ilwisscript/ast/scriptlinenode.cpp \
    ilwisscript/ast/idnode.cpp \
    ilwisscript/ast/assignmentnode.cpp \
    ilwisscript/ast/scriptnode.cpp \
    ilwisscript/ast/variablenode.cpp \
    ilwisscript/ast/expressionnode.cpp \
    ilwisscript/ast/parametersnode.cpp \
    ilwisscript/ast/functionnode.cpp \
    ilwisscript/ast/returnnode.cpp \
    ilwisscript/ast/termnode.cpp \
    ilwisscript/ast/multiplicationnode.cpp \
    ilwisscript/ast/operationnode.cpp \
    ilwisscript/ast/addnode.cpp \
    ilwisscript/ast/relationnode.cpp \
    ilwisscript/ast/commandnode.cpp \
    ilwisscript/ast/functionstatementnode.cpp \
    ilwisscript/ast/whilenode.cpp \
    ilwisscript/ast/breaknode.cpp \
    ilwisscript/ast/valuerangenode.cpp \
    ilwisscript/parserlexer/runtimechecks.cpp \
    ilwisscript/ast/selectornode.cpp \
    ilwisscript/ast/formatter.cpp \
    ilwisscript/ast/domainformatter.cpp \
    ilwisscript/ast/ifnode.cpp \
    ilwisscript/ast/outparametersnode.cpp \
    ilwisscript/ast/selectnode.cpp


HEADERS +=\
    ilwisscript/ilwisscriptmodule.h \
    ilwisscript/calculator_global.h \
    ilwisscript/script.h \
    ilwisscript/parserlexer/ilwisscriptParser.h \
    ilwisscript/parserlexer/IlwisScriptLexer.h \
    ilwisscript/parserlexer/ilwisscript.g \
    ilwisscript/ast/astnode.h \
    ilwisscript/ast/scriptlinenode.h \
    ilwisscript/ast/idnode.h \
    ilwisscript/ast/assignmentnode.h \
    ilwisscript/ast/scriptnode.h \
    ilwisscript/ast/variablenode.h \
    ilwisscript/ast/expressionnode.h \
    ilwisscript/ast/parametersnode.h \
    ilwisscript/ast/functionnode.h \
    ilwisscript/ast/returnnode.h \
    ilwisscript/ast/termnode.h \
    ilwisscript/ast/multiplicationnode.h \
    ilwisscript/ast/operationnode.h \
    ilwisscript/ast/addnode.h \
    ilwisscript/ast/relationnode.h \
    ilwisscript/ast/commandnode.h \
    ilwisscript/ast/functionstatementnode.h \
    ilwisscript/ast/whilenode.h \
    ilwisscript/ast/breaknode.h \
    ilwisscript/ast/valuerangenode.h \
    ilwisscript/ast/selectornode.h \
    ilwisscript/ast/formatters.h \
    ilwisscript/ast/formatter.h \
    ilwisscript/ast/domainformatter.h \
    ilwisscript/ast/ifnode.h \
    ilwisscript/ast/outparametersnode.h \
    ilwisscript/ast/selectnode.h

OTHER_FILES += ilwisscript/ilwisscript.json

INCLUDEPATH += $$PWD/core \
                        ilwisscript \
                        ilwisscript/ast \
                        ilwisscript/internalmethods

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    LIBS +=  -L$$ANTLRLIB -llibantlr
}
unix {
    LIBS += -L$$ANTLRLIB -lantlr3c
}


INCLUDEPATH +=  $$PWD/ilwisscript/parserlexer/include
DEPENDPATH +=   $$PWD/ilwisscript/parserlexer/include

INCLUDEPATH += $$ANTLRLIB/local/include
DEPENDPATH += $$ANTLRLIB//local/include

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

linux{
    dependencies.files = $$LINUXLIB/libantlr3c.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

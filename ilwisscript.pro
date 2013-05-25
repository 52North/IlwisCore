#-------------------------------------------------
#
# Project created by QtCreator 2012-06-14T18:29:31
#
#-------------------------------------------------

CONFIG += plugin
TARGET = ilwisscript

QMAKE_CXXFLAGS += -Wno-unknown-pragmas

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

QT       -= gui
QT += sql

TEMPLATE = lib

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
    ilwisscript/ast/dompartnode.cpp \
    ilwisscript/ast/valuerangenode.cpp \
    ilwisscript/symboltable.cpp \
    ilwisscript/parserlexer/runtimechecks.cpp \
    ilwisscript/ast/formatnode.cpp \
    ilwisscript/ast/modifierpart.cpp \
    ilwisscript/ast/rangedefinitionnode.cpp


HEADERS +=\
    ilwisscript/ilwisscriptmodule.h \
    ilwisscript/calculator_global.h \
    ilwisscript/script.h \
    ilwisscript/parserlexer/IlwisScriptParser.h \
    ilwisscript/parserlexer/IlwisScriptLexer.h \
    ilwisscript/parserlexer/IlwisScript.g \
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
    ilwisscript/ast/dompartnode.h \
    ilwisscript/ast/valuerangenode.h \
    ilwisscript/symboltable.h \
    ilwisscript/ast/formatnode.h \
    ilwisscript/ast/modifierpart.h \
    ilwisscript/ast/rangedefinitionnode.h


INCLUDEPATH += $$PWD/core \
                        ilwisscript \
                        ilwisscript/ast \
                        ilwisscript/internalmethods

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/ilwisscript/parserlexer/include
DEPENDPATH += $$PWD/ilwisscript/parserlexer/include


unix:!symbian|win32: LIBS += -L$$PWD/../libraries/win32debug/ -llibantlr

win32: PRE_TARGETDEPS += $$PWD/../libraries/win32debug/libantlr.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../libraries/win32debug/liblibantlr.a

OTHER_FILES += \
    ilwisscript/ilwisscript.json

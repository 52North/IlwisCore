# Instructions:
#
# For Windows:
# 1. Download the "swigwin" zipfile from http://www.swig.org/download.html
# 2. Unzip "swigwin" to C:\Program Files (x86)\swigwin\
# 3. Optionally (for creating installers of the python extension) install NSIS: http://nsis.sourceforge.net/Download ; the installer is created when chosing "Deploy"
#
# For Linux:
# 1. Install swig
#
# To build an "internal" Python for "pythonscript":
#
# 1. Copy folder C:\Python35\include to <ilwis4_sources>/external/Python35/include
# 2. Copy either libpython35.a or python*.lib from C:\Python35\libs to <ilwis4_sources>/libraries/win32debug/extensions/pythonapi/ and to <ilwis4_sources>/libraries/win32release/extensions/pythonapi/
# 3. Copy folder C:\Python35 to <ilwis4_sources>\output\win32debug\bin\extensions\pythonscript\python\ and to <ilwis4_sources>\output\win32release\bin\extensions\pythonscript\python\ (destination folder is called "python", thus without the version number)
# 4. Build project "pythonapi"
# In the above, replace 35 with the actual version
#
# To build for a random Python version, unrelated to "pythonscript":
#
# 1. Install Python to C:\ (ensure the "standard python folder naming is used", e.g. C:\Python34 , C:\Python35 etc.)
# 2. Build project "pythonapi"
# When multiple python versions are installed on C:\, automatically the newest one is chosen
# When python is available in both C:\ and <ilwis4_sources>/external/, the "external" is preferred. temporarily remove it from "external" if this is unwanted (alternatively, temporarily alter this .pro script)

include(global.pri)

QT += core

TARGET = _ilwisobjects

PYTHONAPI_FOLDER = pythonapi

QMAKE_EXTENSION_SHLIB = pyd

TEMPLATE = lib

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$PYTHONAPI_FOLDER

HEADERS += \
    pythonapi/pythonapi_util.h \
    pythonapi/pythonapi_table.h \
    pythonapi/pythonapi_rastercoverage.h \
    pythonapi/pythonapi_range.h \
    pythonapi/pythonapi_qvariant.h \
    pythonapi/pythonapi_qtGNUTypedefs.h \
    pythonapi/pythonapi_qissuelogger.h \
    pythonapi/pythonapi_pyobject.h \
    pythonapi/pythonapi_pixeliterator.h \
    pythonapi/pythonapi_object.h \
    pythonapi/pythonapi_ilwisobject.h \
    pythonapi/pythonapi_georeference.h \
    pythonapi/pythonapi_geometry.h \
    pythonapi/pythonapi_featureiterator.h \
    pythonapi/pythonapi_featurecoverage.h \
    pythonapi/pythonapi_feature.h \
    pythonapi/pythonapi_extension.h \
    pythonapi/pythonapi_error.h \
    pythonapi/pythonapi_engine.h \
    pythonapi/pythonapi_domain.h \
    pythonapi/pythonapi_coverage.h \
    pythonapi/pythonapi_coordinatesystem.h \
    pythonapi/pythonapi_collection.h \
    pythonapi/pythonapi_catalog.h \
    pythonapi/pythonapi_datadefinition.h \
    pythonapi/pythonapi_rangeiterator.h \
    pythonapi/pythonapi_columndefinition.h \
    pythonapi/pythonapi_domainitem.h \
    pythonapi/pythonapi_vertexiterator.h \
    pythonapi/pythonapi_booleanobject.h

SOURCES += \
    pythonapi/ilwisobjects_wrap.cxx \
    pythonapi/pythonapi_util.cpp \
    pythonapi/pythonapi_table.cpp \
    pythonapi/pythonapi_rastercoverage.cpp \
    pythonapi/pythonapi_range.cpp \
    pythonapi/pythonapi_qvariant.cpp \
    pythonapi/pythonapi_qissuelogger.cpp \
    pythonapi/pythonapi_pyobject.cpp \
    pythonapi/pythonapi_pixeliterator.cpp \
    pythonapi/pythonapi_ilwisobject.cpp \
    pythonapi/pythonapi_georeference.cpp \
    pythonapi/pythonapi_geometry.cpp \
    pythonapi/pythonapi_featureiterator.cpp \
    pythonapi/pythonapi_featurecoverage.cpp \
    pythonapi/pythonapi_feature.cpp \
    pythonapi/pythonapi_extension.cpp \
    pythonapi/pythonapi_error.cpp \
    pythonapi/pythonapi_engine.cpp \
    pythonapi/pythonapi_domain.cpp \
    pythonapi/pythonapi_coverage.cpp \
    pythonapi/pythonapi_coordinatesystem.cpp \
    pythonapi/pythonapi_collection.cpp \
    pythonapi/pythonapi_catalog.cpp \
    pythonapi/pythonapi_datadefinition.cpp \
    pythonapi/pythonapi_rangeiterator.cpp \
    pythonapi/pythonapi_columndefinition.cpp \
    pythonapi/pythonapi_domainitem.cpp \
    pythonapi/pythonapi_vertexiterator.cpp \
    pythonapi/pythonapi_booleanobject.cpp

OTHER_FILES += \
    pythonapi/test.py \
    pythonapi/setup.py \
    pythonapi/ilwisobjects.i \
    pythonapi/ilwisobjects.py \
    pythonapi/paths.py \
    pythonapi/installerPy.nsi \
    pythonapi/LICENSE-2.0.txt \
    pythonapi/README \
    pythonapi/UPDATE \
    pythonapi/CHANGELOG

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
INCLUDEPATH += $$PWD/core \
               $$PWD/../external/geos

PYMINORVERSION = 0
PYVERLIST = 1 2 3 4 5 6 7 8 9
for (VER, PYVERLIST): exists($$PWD/../external/Python3$$VER/include/Python.h) {
    PYMINORVERSION = $$VER
}

equals(PYMINORVERSION, 0) {
    for (VER, PYVERLIST): exists(C:/Python3$$VER/include/Python.h) {
        PYMINORVERSION = $$VER
    }
    equals(PYMINORVERSION, 0) {
        message("Error: Python not found in $$clean_path($$PWD/../external/) or in C:/")
    } else {
        message("Configuring for Python 3.$$PYMINORVERSION found in C:/Python3$$PYMINORVERSION")
        LIBS += -LC:/Python3$$PYMINORVERSION/libs -lpython3$$PYMINORVERSION
        INCLUDEPATH += C:/Python3$$PYMINORVERSION/include/
    }
} else {
    message("Configuring for Python 3.$$PYMINORVERSION found in $$clean_path($$PWD/../external/Python3$$PYMINORVERSION/include)/ and in $$clean_path($$DESTDIR)/")
    exists($$DESTDIR/python3$$PYMINORVERSION*) {
    } else {
        exists($$DESTDIR/libpython3$$PYMINORVERSION*) {
        } else {
            message("Check!! Missing $$clean_path($$DESTDIR/python3$$PYMINORVERSION)*.lib or $$clean_path($$DESTDIR/libpython3$$PYMINORVERSION).a")
        }
    }
    LIBS += -L$$DESTDIR -lpython3$$PYMINORVERSION
    INCLUDEPATH += $$PWD/../external/Python3$$PYMINORVERSION/include/
}

DEPENDPATH += $$PWD/core \
              $$PWD/../external/geos

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

COPY_FILES = ilwisobjects.py test.py README UPDATE CHANGELOG
SOURCE_DIR = $$clean_path($$PWD/$$PYTHONAPI_FOLDER)

linux {
    swig.target = $$SOURCE_DIR/ilwisobjects.py
    swig.commands = swig -python -c++ $$SOURCE_DIR/ilwisobjects.i
    swig.depends = $$SOURCE_DIR/ilwisobjects.i $$SOURCE_DIR/*.h $$SOURCE_DIR/*.cpp
    QMAKE_EXTRA_TARGETS += swig
    PRE_TARGETDEPS += $$SOURCE_DIR/ilwisobjects.py
    DEST_DIR = $$clean_path($$DESTDIR)
    QMAKE_POST_LINK += $$quote(test -d $$DEST_DIR || mkdir -p $$DEST_DIR$$escape_expand(\n\t))
    for(FILE,COPY_FILES){
        QMAKE_POST_LINK += $$quote(cp $$SOURCE_DIR/$$FILE $$DEST_DIR$$escape_expand(\n\t))
    }
    QMAKE_POST_LINK += $$quote(cat $$SOURCE_DIR/paths.py $$DEST_DIR/ilwisobjects.py > $$DEST_DIR/temp.py$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(mv -f $$DEST_DIR/temp.py $$DEST_DIR/__init__.py$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(rm -f $$DEST_DIR/ilwisobjects.py$$escape_expand(\n\t))
    exists($$PWD/../external/Python3$$PYMINORVERSION/include/Python.h) {
        PYTHONSCRIPT_DIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/pythonscript/python/Lib/site-packages/ilwis
        PYTHONSCRIPT_DIR = $$clean_path($$PYTHONSCRIPT_DIR)
        QMAKE_POST_LINK +=$$quote(test -d $$PYTHONSCRIPT_DIR || mkdir -p $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        for(FILE,COPY_FILES){
            QMAKE_POST_LINK +=$$quote(cp $$SOURCE_DIR/$$FILE $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        }
        QMAKE_POST_LINK += $$quote(cp $$DEST_DIR/$${TARGET}.$${QMAKE_EXTENSION_SHLIB} $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        QMAKE_POST_LINK += $$quote(mv -f $$PYTHONSCRIPT_DIR/ilwisobjects.py $$PYTHONSCRIPT_DIR/__init__.py$$escape_expand(\n\t))
    }
}

win32 {
    PYDLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$PYTHONAPI_FOLDER # copy .pyd to output is done manually (not through DLLDESTDIR because that happens too late)
    SOURCE_DIR = $$replace(SOURCE_DIR,/,\\)
    swig.target = $$SOURCE_DIR\\ilwisobjects.py
    swig.commands = $$quote(C:\\Program Files (x86)\\swigwin\\swig.exe) -python -c++ $$SOURCE_DIR\\ilwisobjects.i
    swig.depends = $$SOURCE_DIR\\ilwisobjects.i $$SOURCE_DIR\\*.h $$SOURCE_DIR\\*.cpp
    QMAKE_EXTRA_TARGETS += swig
    PRE_TARGETDEPS += $$SOURCE_DIR\\ilwisobjects.py
    installer_script.files = pythonapi/installerPy.nsi
    installer_script.path = $$PWD/../output/$$PLATFORM$$CONF
    installer.commands = $$quote(C:\\Program Files (x86)\\NSIS\\makensis.exe) /V4 $$PWD/../output/$$PLATFORM$$CONF/installerPy.nsi
    installer.path = $$PWD/../output/$$PLATFORM$$CONF
    INSTALLS += installer_script installer
    DEST_DIR = $$clean_path($$PYDLLDESTDIR)
    DEST_DIR = $$replace(DEST_DIR,/,\\)
    LIB_DIR = $$clean_path($$DESTDIR)
    LIB_DIR = $$replace(LIB_DIR,/,\\)
    QMAKE_POST_LINK +=$$quote(if not exist $$DEST_DIR mkdir $$DEST_DIR$$escape_expand(\n\t))
    QMAKE_POST_LINK +=$$quote(copy /y $$LIB_DIR\\$${TARGET}.$${QMAKE_EXTENSION_SHLIB} $$DEST_DIR$$escape_expand(\n\t))
    for(FILE,COPY_FILES){
        QMAKE_POST_LINK +=$$quote(copy /y $$SOURCE_DIR\\$$FILE $$DEST_DIR$$escape_expand(\n\t))
    }
    QMAKE_POST_LINK += $$quote(copy /y /b $$SOURCE_DIR\\paths.py + $$DEST_DIR\\ilwisobjects.py $$DEST_DIR\\temp.py$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(move /y $$DEST_DIR\\temp.py $$DEST_DIR\\__init__.py$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(del /f $$DEST_DIR\\ilwisobjects.py$$escape_expand(\n\t))
    exists($$PWD/../external/Python3$$PYMINORVERSION/include/Python.h) {
        PYTHONSCRIPT_DIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/pythonscript/python/Lib/site-packages/ilwis
        PYTHONSCRIPT_DIR = $$clean_path($$PYTHONSCRIPT_DIR)
        PYTHONSCRIPT_DIR = $$replace(PYTHONSCRIPT_DIR,/,\\)
        QMAKE_POST_LINK +=$$quote(if not exist $$PYTHONSCRIPT_DIR mkdir $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        for(FILE,COPY_FILES){
            QMAKE_POST_LINK +=$$quote(copy /y $$SOURCE_DIR\\$$FILE $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        }
        QMAKE_POST_LINK += $$quote(copy /y $$DEST_DIR\\$${TARGET}.$${QMAKE_EXTENSION_SHLIB} $$PYTHONSCRIPT_DIR$$escape_expand(\n\t))
        QMAKE_POST_LINK += $$quote(move /y $$PYTHONSCRIPT_DIR\\ilwisobjects.py $$PYTHONSCRIPT_DIR\\__init__.py$$escape_expand(\n\t))
    }
}

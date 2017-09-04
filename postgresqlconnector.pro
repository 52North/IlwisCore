##-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

CONFIG += plugin
TARGET = postgresqlconnector

include(global.pri)

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

QT       -= gui

TEMPLATE = lib

DEFINES += POSTGRESSQLCONNECTOR_LIBRARY

OTHER_FILES += \
    postgresqlconnector/postgresqlconnector.json \ 
    postgresqlconnector/scratch_pad.txt


		

win32{
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
}

linux{
    LIBS +=  -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$GEOSLIB/ -lgeos-3.4.2
}

win32:CONFIG(release, debug|release): {
    QMAKE_CXXFLAGS_RELEASE += -O2
}

INCLUDEPATH +=  $$PWD/core \
                $$PWD/../external/geos \
                $$PWD/../external/postgresql \

DEPENDPATH +=   $$PWD/core \
                $$PWD/../external/geos \

HEADERS += \
    postgresqlconnector/postgresqlconnector.h \
    postgresqlconnector/postgresqlmodule.h \
    postgresqlconnector/postgresqlobjectfactory.h \
    postgresqlconnector/postgresqlfeatureconnector.h \
    postgresqlconnector/postgresqltableconnector.h \
    postgresqlconnector/postgresqlcatalogexplorer.h \
    postgresqlconnector/postgresqldatabaseutil.h \
    postgresqlconnector/postgresqltableloader.h \
    postgresqlconnector/postgresqlfeaturecoverageloader.h \
    postgresqlconnector/sqlstatementhelper.h \
    postgresqlconnector/postgresqlconnection.h \
    postgresqlconnector/postgresqlrasterconnector.h \
    postgresqlconnector/postgresqlparameters.h

SOURCES += \
    postgresqlconnector/postgresqlconnector.cpp \
    postgresqlconnector/postgresqlmodule.cpp \
    postgresqlconnector/postgresqlobjectfactory.cpp \
    postgresqlconnector/postgresqlfeatureconnector.cpp \
    postgresqlconnector/postgresqltableconnector.cpp \
    postgresqlconnector/postgresqlcatalogexplorer.cpp \
    postgresqlconnector/postgresqltableloader.cpp \
    postgresqlconnector/postgresqlfeaturecoverageloader.cpp \
    postgresqlconnector/sqlstatementhelper.cpp \
    postgresqlconnector/postgresqlconnection.cpp \
    postgresqlconnector/PostgresqlDatabaseUtil.cpp \
    postgresqlconnector/postgresqlrasterconnector.cpp \
    postgresqlconnector/postgresqlparameters.cpp

linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T19:05:01
#
#-------------------------------------------------

QT = sql network xmlpatterns

CONFIG += plugin
TARGET = wfsconnector

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

TEMPLATE = lib

DEFINES += WFSCONNECTOR_LIBRARY

SOURCES += \
    wfsconnector/wfsobjectfactory.cpp \
    wfsconnector/wfsmodule.cpp \
    wfsconnector/wfsfeatureconnector.cpp \
    wfsconnector/wfs.cpp \
    wfsconnector/wfsresponse.cpp \
    wfsconnector/wfsfeature.cpp \
    wfsconnector/wfscapabilitiesparser.cpp \
    wfsconnector/wfsfeatureparser.cpp \
    wfsconnector/wfsfeaturedescriptionparser.cpp \
    wfsconnector/wfsparsingcontext.cpp \
    wfsconnector/wfscatalogexplorer.cpp \
    wfsconnector/wfsconnection.cpp

HEADERS += \
    wfsconnector/wfsobjectfactory.h \
    wfsconnector/wfsmodule.h \
    wfsconnector/wfsfeatureconnector.h \
    wfsconnector/wfs.h \
    wfsconnector/wfsconnector_global.h \
    wfsconnector/wfsresponse.h \
    wfsconnector/wfsfeature.h \
    wfsconnector/wfscapabilitiesparser.h \
    wfsconnector/wfsfeatureparser.h \
    wfsconnector/wfsfeaturedescriptionparser.h \
    wfsconnector/wfsutils.h \
    wfsconnector/wfsparsingcontext.h \
    wfsconnector/wfscatalogexplorer.h \
    wfsconnector/wfsconnection.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
                                              -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
                                              -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos


linux{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore \
        -L$$GEOSLIB/ -lgeos-3.4.2
}

INCLUDEPATH += $$PWD/../external/geos

OTHER_FILES += \
    wfsconnector/wfsconnector.json \
    wfsconnector/scratch_pad.txt

resources.files = \
    wfsconnector/resources/codes_with_latlon_order.txt

resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET/resources


linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}

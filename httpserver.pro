#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T13:18:13
#
#-------------------------------------------------

QT = network xmlpatterns

CONFIG += plugin
TARGET = httpserver

include(global.pri)

TEMPLATE = lib

DEFINES += HTTPSERVER_LIBRARY

SOURCES += \ 
    httpserver/httpservermodule.cpp \
    httpserver/httpserver.cpp \
    httpserver/httpserver/staticfilecontroller.cpp \
    httpserver/httpserver/httpsessionstore.cpp \
    httpserver/httpserver/httpsession.cpp \
    httpserver/httpserver/httpresponse.cpp \
    httpserver/httpserver/httprequesthandler.cpp \
    httpserver/httpserver/httprequest.cpp \
    httpserver/httpserver/httplistener.cpp \
    httpserver/httpserver/httpcookie.cpp \
    httpserver/httpserver/httpconnectionhandlerpool.cpp \
    httpserver/httpserver/httpconnectionhandler.cpp \
    httpserver/service/qtunixsocket.cpp \
    httpserver/service/qtservice.cpp \
    httpserver/templateengine/templateloader.cpp \
    httpserver/templateengine/templatecache.cpp \
    httpserver/templateengine/template.cpp \
    httpserver/httpserver/requestmapper.cpp \
    httpserver/httpserver/serverstarter.cpp \
    httpserver/service/serverthread.cpp \
    httpserver/service/qtunixserversocket.cpp \
    httpserver/requesthandlerfactory.cpp

HEADERS += \ 
    httpserver/httpservermodule.h \
    httpserver/httpserver.h \
    httpserver/httpserver/staticfilecontroller.h \
    httpserver/httpserver/httpsessionstore.h \
    httpserver/httpserver/httpsession.h \
    httpserver/httpserver/httpresponse.h \
    httpserver/httpserver/httprequesthandler.h \
    httpserver/httpserver/httprequest.h \
    httpserver/httpserver/httplistener.h \
    httpserver/httpserver/httpcookie.h \
    httpserver/httpserver/httpconnectionhandlerpool.h \
    httpserver/httpserver/httpconnectionhandler.h \
    httpserver/service/qtunixsocket.h \
    httpserver/service/qtunixserversocket.h \
    httpserver/service/qtservice_p.h \
    httpserver/service/qtservice.h \
    httpserver/templateengine/templateloader.h \
    httpserver/templateengine/templatecache.h \
    httpserver/templateengine/template.h \
    httpserver/httpserver/requestmapper.h \
    httpserver/httpserver/serverstarter.h \
    httpserver/service/serverthread.h \
    httpserver/requesthandlerfactory.h

OTHER_FILES += \
    httpserver/httpserver.json

INCLUDEPATH +=  $$PWD/extensions/httpserver

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF

linux{
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/
    INSTALLS += target
}


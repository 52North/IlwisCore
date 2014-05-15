QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
QMAKE_CXXFLAGS_RELEASE += -O2
}

PLATFORM = generic
win32{
    PLATFORM = win32
    BOOST=../external
}
linux{
    BOOST=/usr/include
    GEOSINCL=/usr/include
    GEOSLIB=/usr/lib
}
EXTERNAL=../external

QT += sql

plugin{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
}shared{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
}else {
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
}

INCLUDEPATH += core \
                core/ilwisobjects \
                core/ilwisobjects/geometry \
                core/util \
                core/ilwisobjects/geometry/geodeticdatum \
                core/ilwisobjects/geometry/projection \
                core/ilwisobjects/geometry/coordinatesystem \
                core/ilwisobjects/geometry/georeference \
                core/ilwisobjects/coverage \
                core/ilwisobjects/table \
                core/ilwisobjects/operation \
                core/catalog \
                core/ilwisobjects/domain \
                $$BOOST \
                $$EXTERNAL

win32:{
LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos
INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos
}
linux:{
INCLUDEPATH += $$GEOSINCL
DEPENDPATH += $$GEOSINCL
LIBS += -L$$GEOSLIB/ -lgeos-3.4.2
}

INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos


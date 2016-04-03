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
    SHAREDEXT=dll
    PREFIXSHARED=
    LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -llibgeos


    INCLUDEPATH += $$PWD/../external/geos
    DEPENDPATH += $$PWD/../external/geos
}
linux{
    BOOST=/usr/include
    GEOSINCL=/usr/include
    GEOSLIB=/usr/lib
    SHAREDEXT=so
    PREFIXSHARED=lib
    INCLUDEPATH += $$GEOSINCL
    DEPENDPATH += $$GEOSINCL
    LIBS += -L$$GEOSLIB/ -lgeos-3.4.2 \
            -L/usr/lib -lgsl \
            -L/usr/lib -lgslcblas
}

linux-g++{
   !contains(QT_ARCH, x86_64){
       LINUXLIB=/usr/lib/i386-linux-gnu/
    } else {
       LINUXLIB=/usr/lib/x86_64-linux-gnu/
   }
}

EXTERNAL=../external

QT += sql network concurrent

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
                core/ilwisobjects/workflow \
                core/ilwisobjects/representation \
                core/catalog \
                core/ilwisobjects/domain \
                $$BOOST \
                $$EXTERNAL



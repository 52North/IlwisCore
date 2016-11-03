QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -fext-numeric-literals
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
}
linux{
    BOOST=/usr/include
    GEOSINCL=/usr/include
    GEOSLIB=/usr/lib
    SHAREDEXT=so
    PREFIXSHARED=lib
    LIBS += -L/usr/lib -lgsl \
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
                core/ilwisobjects/operation/model \
                core/ilwisobjects/operation \
                core/ilwisobjects/operation\modeller \
                core/ilwisobjects/workflow \
                core/ilwisobjects/representation \
                core/catalog \
                core/ilwisobjects/domain \
                core/geos/include\
                $$BOOST \
                $$EXTERNAL



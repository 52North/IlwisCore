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
}

OUTDIR = $$PWD/output/$$CONF
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
    LIBS += -L$$GEOSLIB/ -lgeos-3.4.2
}

QT += sql network

INCLUDEPATH += ../IlwisCore/core \
                ../IlwisCore/core/ilwisobjects \
                ../IlwisCore/core/ilwisobjects/geometry \
                ../IlwisCore/core/util \
                ../IlwisCore/core/ilwisobjects/geometry/geodeticdatum \
                ../IlwisCore/core/ilwisobjects/geometry/projection \
                ../IlwisCore/core/ilwisobjects/geometry/coordinatesystem \
                ../IlwisCore/core/ilwisobjects/geometry/georeference \
                ../IlwisCore/core/ilwisobjects/coverage \
                ../IlwisCore/core/ilwisobjects/table \
                ../IlwisCore/core/ilwisobjects/operation \
                ../IlwisCore/core/catalog \
                ../IlwisCore/core/ilwisobjects/domain \
                $$BOOST



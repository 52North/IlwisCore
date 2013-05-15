QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-sign-compare

CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
}

PLATFORM = generic
win32{
    PLATFORM = win32
}
BOOST=../external

QT += sql

plugin{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
}dll{
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
}else {
DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin
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
                $$BOOST




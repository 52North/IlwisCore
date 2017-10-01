#-------------------------------------------------
#
# Project created by QtCreator 2013-04-22T08:25:30
#
#-------------------------------------------------
CONFIG += plugin

QT       += sql

TARGET = baseoperations
TEMPLATE = lib

include(global.pri)

DEFINES += BASEOPERATIONS_LIBRARY

HEADERS += \
    baseoperations/baseoperationsmodule.h \
    baseoperations/baseoperations_global.h \
    baseoperations/geometry/resampleraster.h \
    baseoperations/data/rasvalue.h \
    baseoperations/data/iffraster.h \
    baseoperations/math/binarylogical.h \
    baseoperations/math/unarymath.h \
    baseoperations/math/unarymathoperations.h \
    baseoperations/data/assignment.h \
    baseoperations/data/selection.h \
    baseoperations/geometry/pixel2coord.h \
    baseoperations/geometry/fpixel.h \
    baseoperations/geometry/fcoordinate.h \
    baseoperations/geometry/coord2pixel.h \
    baseoperations/util/text2output.h \
    baseoperations/geometry/gridsize.h \
    baseoperations/util/stringoperations.h \
    baseoperations/data/ifoperation.h \
    baseoperations/data/iffeature.h \
    baseoperations/data/selectionfeatures.h \
    baseoperations/math/binarymathraster.h \
    baseoperations/math/binarymathfeature.h \
    baseoperations/geometry/mastergeoreference.h \
    baseoperations/data/selectiontable.h \
    baseoperations/math/binarymathtable.h \
    baseoperations/data/setvaluerange.h \
    baseoperations/geometry/spatialrelation.h \
    baseoperations/util/workingcatalog.h \
    baseoperations/math/unarymathtable.h \
    baseoperations/math/aggregation.h \
    baseoperations/math/columnunaryoperation.h \
    baseoperations/math/unarymathrasterandnumber.h \
    baseoperations/data/convertcolumndomain.h \
    baseoperations/creators/createnumericdomain.h \
    baseoperations/creators/createthematicdomain.h \
    baseoperations/creators/createidentifierdomain.h \
    baseoperations/creators/createintervaldomain.h \
    baseoperations/creators/createpalettedomain.h \
    baseoperations/creators/createcornersgeoreference.h \
    baseoperations/creators/createprojectedcoordinatesystem.h \
    baseoperations/util/testoperation.h \
    baseoperations/math/columnjoin.h \
    baseoperations/data/saveas.h \
    baseoperations/creators/createrastercoverage.h \
    baseoperations/conditions/junction.h \
    baseoperations/conditions/numbercondition.h \
    baseoperations/data/aggregatetable.h \
    baseoperations/data/selectionbase.h \
    baseoperations/data/setattributetabel.h \
    baseoperations/conditions/compare.h \
    baseoperations/data/property.h \
    baseoperations/data/connect.h \
    baseoperations/conditions/rangeoperation.h \
    baseoperations/creators/createcombinationmatrix.h \
    baseoperations/math/mapcalc.h \
    baseoperations/data/tablevalue.h \
    baseoperations/data/tablevaluebyprimarykey.h \
    baseoperations/data/setvariable.h \
    baseoperations/math/calculatoroperation.h \
    baseoperations/math/tabcalc.h \
    baseoperations/conditions/iscompatible.h \
    baseoperations/data/serviceobject.h \
    baseoperations/data/addrasterband.h \
    baseoperations/creators/createtable.h \
    baseoperations/data/addcolumn.h \
    baseoperations/data/copycolumn.h \
    baseoperations/creators/createboundsonlycsy.h \
    baseoperations/creators/createfeaturecoverage.h
    baseoperations/data/setattributetabel.h

SOURCES += \
    baseoperations/baseoperationsmodule.cpp \
    baseoperations/geometry/resampleraster.cpp \
    baseoperations/data/rasvalue.cpp \
    baseoperations/data/iffraster.cpp \
    baseoperations/math/binarylogical.cpp \
    baseoperations/math/unarymath.cpp \
    baseoperations/math/unarymathoperations.cpp \
    baseoperations/data/assignment.cpp \
    baseoperations/data/selection.cpp \
    baseoperations/geometry/pixel2coord.cpp \
    baseoperations/geometry/fpixel.cpp \
    baseoperations/geometry/fcoordinate.cpp \
    baseoperations/geometry/coord2pixel.cpp \
    baseoperations/util/text2output.cpp \
    baseoperations/geometry/gridsize.cpp \
    baseoperations/util/stringoperations.cpp \
    baseoperations/data/ifoperation.cpp \
    baseoperations/data/iffeature.cpp \
    baseoperations/data/selectionfeatures.cpp \
    baseoperations/math/binarymathraster.cpp \
    baseoperations/math/binarymathfeature.cpp \
    baseoperations/geometry/mastergeoreference.cpp \
    baseoperations/data/selectiontable.cpp \
    baseoperations/math/binarymathtable.cpp \
    baseoperations/data/setvaluerange.cpp \
    baseoperations/geometry/spatialrelation.cpp \
    baseoperations/util/workingcatalog.cpp \
    baseoperations/math/unarymathtable.cpp \
    baseoperations/math/unarymathrasterandnumber.cpp \
    baseoperations/data/convertcolumndomain.cpp \
    baseoperations/math/aggregation.cpp \
    baseoperations/math/columnunaryoperation.cpp \
    baseoperations/creators/createnumericdomain.cpp \
    baseoperations/creators/createthematicdomain.cpp \
    baseoperations/creators/createidentifierdomain.cpp \
    baseoperations/creators/createintervaldomain.cpp \
    baseoperations/creators/createpalettedomain.cpp \
    baseoperations/creators/createcornersgeoreference.cpp \
    baseoperations/creators/createprojectedcoordinatesystem.cpp \
    baseoperations/util/testoperation.cpp \
    baseoperations/math/columnjoin.cpp \
    baseoperations/data/saveas.cpp \
    baseoperations/creators/createrastercoverage.cpp \
    baseoperations/conditions/junction.cpp \
    baseoperations/conditions/numbercondition.cpp \
    baseoperations/data/aggregatetable.cpp \
    baseoperations/data/selectionbase.cpp \
    baseoperations/data/setattributetabel.cpp \
    baseoperations/conditions/compare.cpp \
    baseoperations/data/property.cpp \
    baseoperations/data/connect.cpp \
    baseoperations/conditions/rangeoperation.cpp \
    baseoperations/creators/createcombinationmatrix.cpp \
    baseoperations/math/mapcalc.cpp \
    baseoperations/data/tablevalue.cpp \
    baseoperations/data/tablevaluebyprimarykey.cpp \
    baseoperations/data/setvariable.cpp \
    baseoperations/math/calculatoroperation.cpp \
    baseoperations/math/tabcalc.cpp \
    baseoperations/conditions/iscompatible.cpp \
    baseoperations/data/serviceobject.cpp \
    baseoperations/data/addrasterband.cpp \
    baseoperations/creators/createtable.cpp \
    baseoperations/data/addcolumn.cpp \
    baseoperations/data/copycolumn.cpp \
    baseoperations/creators/createboundsonlycsy.cpp \
    baseoperations/creators/createfeaturecoverage.cpp
    baseoperations/data/setattributetabel.cpp


OTHER_FILES += \
    baseoperations/baseoperations.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    #LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET -lgsl.dll
    #INCLUDEPATH += $$PWD/../external/gsl
}

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

linux{
    LIBS += -L/usr/lib -lgsl
    INCLUDEPATH += /usr/include/gsl
    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target
}


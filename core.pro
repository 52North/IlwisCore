#-------------------------------------------------
#
# Project created by QtCreator 2011-12-21T13:34:30
#
#-------------------------------------------------

include(global.pri)

QT       += network sql xml

TARGET = ilwiscore
TEMPLATE = lib

DEFINES += CORE_LIBRARY


SOURCES += core/kernel.cpp \
    core/version.cpp \
    core/module.cpp \
    core/errorobject.cpp \
    core/abstractfactory.cpp \
    core/ilwisobjects/ilwisobject.cpp \
    core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.cpp \
    core/ilwisobjects/geometry/georeference/georeference.cpp \
    core/ilwisobjects/geometry/georeference/simpelgeoreference.cpp \
    core/ilwisobjects/geometry/georeference/cornersgeoreference.cpp \
    core/ilwisobjects/coverage/coverage.cpp \
    core/publicdatabase.cpp \
    core/ilwisobjects/ilwisobjectfactory.cpp \
    core/issuelogger.cpp \
    core/identity.cpp \
    core/ilwisobjects/domain/domain.cpp \
    core/ilwisobjects/domain/thematicitem.cpp \
    core/util/range.cpp \
    core/catalog/catalog.cpp \
    core/catalog/catalogconnector.cpp \
    core/connectorfactory.cpp \
    core/ilwiscontext.cpp \
    core/catalog/mastercatalog.cpp \
    core/catalog/catalogconnectorfactory.cpp \
    core/catalog/catalogquery.cpp \
    core/catalog/resource.cpp \
    core/ilwisobjects/ilwisobjectconnector.cpp \
    core/ilwisobjects/geometry/coordinatesystem/conventionalcoordinatesystem.cpp \
    core/util/juliantime.cpp \
    core/ilwisobjects/domain/numericdomain.cpp \
    core/ilwisobjects/domain/identifierrange.cpp \
    core/ilwisobjects/domain/identifieritem.cpp \
    core/ilwisobjects/domain/itemrange.cpp \
    core/util/angle.cpp \
    core/ilwisobjects/coverage/featurecoverage.cpp \
    core/ilwisobjects/coverage/feature.cpp \
    core/catalog/filecatalogconnector.cpp \
    core/util/size.cpp \
    core/ilwisobjects/coverage/grid.cpp \
    core/ilwisobjects/coverage/pixeliterator.cpp \
    core/util/numericrange.cpp \
    core/ilwisobjects/table/flattable.cpp \
    core/ilwisobjects/table/databasetable.cpp \
    core/ilwisobjects/table/columndefinition.cpp \
    core/ilwisobjects/coverage/featureiterator.cpp \
    core/ilwisobjects/table/basetable.cpp \
    core/ilwisobjects/table/attributerecord.cpp \
    core/ilwisobjects/coverage/featurefactory.cpp \
    core/ilwisobjects/geometry/coordinatesystem/projectionimplementation.cpp \
    core/ilwisobjects/geometry/coordinatesystem/projection.cpp \
    core/ilwisobjects/geometry/coordinatesystem/proj4parameters.cpp \
    core/ilwisobjects/geometry/coordinatesystem/ellipsoid.cpp \
    core/ilwisobjects/geometry/coordinatesystem/geodeticdatum.cpp \
    core/ilwisobjects/operation/operation.cpp \
    core/ilwisobjects/operation/operationmetadata.cpp \
    core/ilwisobjects/operation/operationExpression.cpp \
    core/ilwisobjects/operation/commandhandler.cpp \
    core/ilwisobjects/coverage/blockiterator.cpp \
    core/util/locker.cpp \
    core/ilwisobjects/domain/datadefinition.cpp \
    core/ilwisobjects/geometry/georeference/ctpgeoreference.cpp \
    core/ilwisobjects/geometry/georeference/controlpoint.cpp \
    core/ilwisobjects/geometry/georeference/planarctpgeoreference.cpp \
    core/util/mathhelper.cpp \
    core/ilwisobjects/domain/textdomain.cpp \
    core/ilwisobjects/geometry/georeference/georefadapter.cpp \
    core/ilwisobjects/operation/symboltable.cpp \
    core/util/linerasterizer.cpp \
    core/ilwisobjects/operation/operationhelpergrid.cpp \
    core/ilwisobjects/operation/operationhelper.cpp \
    core/ilwisobjects/operation/operationhelperfeatures.cpp \
    core/ilwisobjects/geometry/georeference/georefimplementation.cpp \
    core/ilwisobjects/geometry/georeference/georefimplementationfactory.cpp \
    core/ilwisobjects/domain/coordinatedomain.cpp \
    core/ilwisobjects/domain/numericitem.cpp \
    core/ilwisobjects/domain/numericitemrange.cpp \
    core/ilwisobjects/coverage/indexslicer.cpp \
    core/ilwisobjects/coverage/rastercoverage.cpp \
    core/ilwisobjects/coverage/rasterinterpolator.cpp \
    core/ilwisobjects/operation/logicalexpressionparser.cpp \
    core/ilwisobjects/table/tableselector.cpp \
    core/ilwisobjects/table/tablemerger.cpp \
    core/ilwisobjects/domain/domainmerger.cpp \
    core/util/tranquilizer.cpp \
    core/ilwisobjects/operation/numericoperation.cpp \
    core/catalog/containerconnector.cpp \
    core/catalog/folderconnector.cpp \
    core/util/coordinate.cpp \
    core/ilwisobjects/geometry/coordinatesystem/csytransform.cpp \
    core/ilwisobjects/coverage/geometryhelper.cpp \
    core/ilwisobjects/operation/rasterfilter.cpp

HEADERS += core/kernel.h\
      core/kernel_global.h \
    core/version.h \
    core/module.h \
    core/ilwis.h \
    core/errorobject.h \
    core/factory.h \
    core/abstractfactory.h \
    core/util/numericrange.h \
    core/ilwisobjects/ilwisdata.h \
    core/ilwisobjects/ilwisobject.h \
    core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h \
    core/ilwisobjects/geometry/georeference/georeference.h \
    core/ilwisobjects/geometry/georeference/simpelgeoreference.h \
    core/ilwisobjects/geometry/georeference/cornersgeoreference.h \
    core/ilwisobjects/coverage/coverage.h \
    core/publicdatabase.h \
    core/ilwisobjects/ilwisobjectfactory.h \
    core/issuelogger.h \
    core/identity.h \
    core/ilwisobjects/table/table.h \
    core/ilwisobjects/domain/domain.h \
    core/ilwisobjects/domain/thematicitem.h \
    core/util/range.h \
    core/catalog/catalog.h \
    core/catalog/catalogconnector.h \
    core/connectorfactory.h \
    core/connectorinterface.h \
    core/ilwiscontext.h \
    core/catalog/mastercatalog.h \
    core/catalog/catalogconnectorfactory.h \
    core/catalog/catalogquery.h \
    core/catalog/resource.h \
    core/ilwisobjects/ilwisobjectconnector.h \
    core/ilwisobjects/geometry/coordinatesystem/conventionalcoordinatesystem.h \
    core/util/juliantime.h \
    core/ilwisobjects/domain/numericdomain.h \
    core/ilwisobjects/domain/itemdomain.h \
    core/ilwisobjects/domain/identifierrange.h \
    core/ilwisobjects/domain/identifieritem.h \
    core/ilwisobjects/domain/domainitem.h \
    core/util/memorymanager.h \
    core/util/numericrange.h \
    core/ilwisobjects/domain/itemrange.h \
    core/util/serializationoptions.h \
    core/ilwisobjects/coverage/blockiterator.h \
    core/util/angle.h \
    core/util/box.h \
    core/ilwisobjects/coverage/pixeliterator.h \
    core/ilwisobjects/coverage/feature.h \
    core/ilwisobjects/coverage/featurecoverage.h \
    core/catalog/filecatalogconnector.h \
    core/util/containerstatistics.h \
    core/ilwisobjects/coverage/grid.h \
    core/util/size.h \
    core/ilwisobjects/table/flattable.h \
    core/ilwisobjects/table/databasetable.h \
    core/ilwisobjects/table/columndefinition.h \
    core/ilwisobjects/coverage/featureiterator.h \
    core/ilwisobjects/table/basetable.h \
    core/ilwisobjects/table/attributerecord.h \
    core/ilwisobjects/coverage/featurefactory.h \
    core/util/errmessages.h \
    core/ilwisobjects/geometry/coordinatesystem/ProjectionImplementation.h \
    core/ilwisobjects/geometry/coordinatesystem/projectionfactory.h \
    core/ilwisobjects/geometry/coordinatesystem/projection.h \
    core/ilwisobjects/geometry/coordinatesystem/proj4parameters.h \
    core/ilwisobjects/geometry/coordinatesystem/ellipsoid.h \
    core/ilwisobjects/geometry/coordinatesystem/geodeticdatum.h \
    core/ilwisobjects/operation/operation.h \
    core/ilwisobjects/operation/operationmetadata.h \
    core/ilwisobjects/operation/operationExpression.h \
    core/ilwisobjects/operation/operationconnector.h \
    core/ilwisobjects/operation/commandhandler.h \
    core/util/locker.h \
    core/ilwisobjects/coverage/raster.h \
    core/ilwisobjects/operation/ilwisoperation.h \
    core/ilwisobjects/domain/datadefinition.h \
    core/util/geometries.h \
    core/ilwisobjects/geometry/georeference/ctpgeoreference.h \
    core/ilwisobjects/geometry/georeference/controlpoint.h \
    core/ilwisobjects/geometry/georeference/planarctpgeoreference.h \
    core/util/mathhelper.h \
    core/ilwisobjects/domain/textdomain.h \
    core/ilwisobjects/geometry/georeference/georefadapter.h \
    core/ilwisobjects/operation/symboltable.h \
    core/util/linerasterizer.h \
    core/ilwisobjects/operation/operationhelpergrid.h \
    core/ilwisobjects/operation/operationhelper.h \
    core/ilwisobjects/operation/operationhelperfeatures.h \
    core/ilwisobjects/geometry/georeference/georefimplementation.h \
    core/ilwisobjects/geometry/georeference/georefimplementationfactory.h \
    core/ilwisobjects/domain/coordinatedomain.h \
    core/ilwisobjects/domain/numericitem.h \
    core/ilwisobjects/domain/numericitemrange.h \
    core/ilwisobjects/coverage/indexslicer.h \
    core/ilwisobjects/coverage/rastercoverage.h \
    core/ilwisobjects/coverage/rasterinterpolator.h \
    core/ilwisobjects/domain/itemiterator.h \
    core/ilwisobjects/operation/logicalexpressionparser.h \
    core/ilwisobjects/table/tableselector.h \
    core/ilwisobjects/table/tablemerger.h \
    core/ilwisobjects/domain/domainmerger.h \
    core/util/tranquilizer.h \
    core/ilwisobjects/operation/numericoperation.h \
    core/catalog/containerconnector.h \
    core/catalog/folderconnector.h \
    core/util/location.h \
    core/util/coordinate.h \
    core/ilwisobjects/geometry/coordinatesystem/csytransform.h \
    core/ilwisobjects/coverage/geometryhelper.h \
    core/ilwisobjects/operation/rasterfilter.h






win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/win32release/ -llibgeos
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/win32debug/ -llibgeos

INCLUDEPATH += $$PWD/../external/geos
DEPENDPATH += $$PWD/../external/geos


resources.files = resources/datums.csv \
        resources/ellisoids.csv \
        resources/datums.csv \
        resources/numericdomains.csv \
        resources/epsg.pcs \
        resources/projections.csv \
        resources/referencesystems.csv \
        resources/filters.csv

resources.path = $$PWD/../output/$$PLATFORM$$CONF/bin/resources

INSTALLS += resources


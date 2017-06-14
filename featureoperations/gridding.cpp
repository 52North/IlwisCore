#include <functional>
#include <future>
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/GeometryFactory.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "gridding.h"

using namespace Ilwis;
using namespace FeatureOperations;

Gridding::Gridding()
{
}

Gridding::Gridding(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool Gridding::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    for(int fx=0; fx < _xsize; ++fx) {
        for(int fy=0; fy < _ysize; ++fy) {
            Coordinate c1(_top + std::vector<double>{_cellXSize * fx, _cellYSize * fy });
            Coordinate c2(_top +std::vector<double>{_cellXSize * (fx+1), _cellYSize * fy });
            Coordinate c3(_top + std::vector<double>{_cellXSize * (fx+1), _cellYSize * (fy+1) });
            Coordinate c4(_top + std::vector<double>{_cellXSize * fx, _cellYSize * (fy+1) });
            geos::geom::CoordinateSequence *coords = new geos::geom::CoordinateArraySequence();
            coords->add(c1);
            coords->add(c2);
            coords->add(c3);
            coords->add(c4);
            coords->add(c1);
            geos::geom::LinearRing *outer = _outfeatures->geomfactory()->createLinearRing(coords);
            geos::geom::Geometry *pol = _outfeatures->geomfactory()->createPolygon(outer, 0);
            _outfeatures->newFeature(pol);
        }
    }
    _outfeatures->setAttributes(_attTable);

    QVariant value;
    value.setValue<IFeatureCoverage>(_outfeatures);
    logOperation(_outfeatures, _expression);
    ctx->setOutput(symTable, value, _outfeatures->name(), itFEATURE,_outfeatures->resource());
    return true;
}

OperationImplementation *Gridding::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new  Gridding(metaid, expr)   ;
}

quint64 Gridding::createMetadata()
{
    QString url = QString("ilwis://operations/gridding");
    Resource resource(QUrl(url), itSINGLEOPERATION);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","gridding");
    resource.addProperty("syntax","gridding(coordinatesyste,top-coordinate,x-cell-size, y-cell-size, horizontal-cells, vertical-cells)");
    resource.addProperty("description",TR("generates a new featurecoverage(polygons) were the polygons form a rectangular grid"));
    resource.addProperty("inparameters","6");
    resource.addProperty("pin_1_type", itCOORDSYSTEM);
    resource.addProperty("pin_1_name", TR("coordinate-system"));
    resource.addProperty("pin_1_desc",TR("The coordinate system of the to be created polygon coverage"));
    resource.addProperty("pin_2_type", itCOORDINATE);
    resource.addProperty("pin_2_name", TR("top corner"));
    resource.addProperty("pin_2_desc",TR("The top corner of the polygonmap expressed in coordinates of the coordinate system"));
    resource.addProperty("pin_3_type", itDOUBLE);
    resource.addProperty("pin_3_name", TR("X cell size"));
    resource.addProperty("pin_3_desc",TR("Size in the x direction of a cell in the grid expressed in untis of the coordinate system"));
    resource.addProperty("pin_4_type", itDOUBLE);
    resource.addProperty("pin_4_name", TR("Y cell size"));
    resource.addProperty("pin_4_desc",TR("Size in the y direction of a cell in the grid expressed in untis of the coordinate system"));
    resource.addProperty("pin_5_type", itINTEGER);
    resource.addProperty("pin_5_name", TR("Horizontal cells"));
    resource.addProperty("pin_5_desc",TR("Number of cells in the x directions"));
    resource.addProperty("pin_6_type", itINTEGER);
    resource.addProperty("pin_6_name", TR("Vertical cells"));
    resource.addProperty("pin_6_desc",TR("Number of cells in the y directions"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itPOLYGON);
    resource.addProperty("pout_1_name", TR("output polygon coverage"));
    resource.addProperty("pout_1_desc",TR("output polygon coverage"));
    resource.addProperty("keyword","vector,features");

    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

OperationImplementation::State Gridding::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString csyName =  _expression.parm(0).value();
    if (!_csy.prepare(csyName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,csyName,"" );
        return sPREPAREFAILED;
    }

    QString name = _expression.parm(1).value();
    QVariant var = st.getValue(name);
    _top = var.value<Coordinate>();
    if (!_top.isValid() || _top.is0()) {
        _top = var.value<Coordinate>();
    }
    if (!_top.isValid() || _top.is0()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value",name);
        return sPREPAREFAILED;
    }
    bool ok;
    _cellXSize = _expression.parm(2).value().toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","3");
        return sPREPAREFAILED;
    }

    _cellYSize = _expression.parm(3).value().toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","4");
        return sPREPAREFAILED;
    }

    _xsize = _expression.parm(4).value().toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","5");
        return sPREPAREFAILED;
    }

    _ysize = _expression.parm(5).value().toDouble(&ok);
    if ( !ok ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"parameter value","6");
        return sPREPAREFAILED;
    }
    QString outputName = _expression.parm(0,false).value();
     Resource resource = outputName != sUNDEF ? Resource(INTERNAL_CATALOG + "/" + outputName, itFLATTABLE) : Resource(itFLATTABLE);
    _attTable.prepare(resource);
    IDomain covdom;
    if (!covdom.prepare("count")){
        return sPREPAREFAILED;
    }
    _outfeatures.prepare();
    _outfeatures->coordinateSystem(_csy);
    Envelope env(_top, _top + std::vector<double>{_cellXSize * _xsize, _cellYSize * _ysize });
    _outfeatures->envelope(env);
    return sPREPARED;
}

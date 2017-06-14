#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "geos/geom/Point.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/operation/polygonize/Polygonizer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"
#include "table.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelperfeatures.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "geometryhelper.h"
#include "geos/geom/LineString.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"
#include "polygon2line.h"
#include "line2polygon.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(Line2Polygon)

Line2Polygon::Line2Polygon()
{

}

Line2Polygon::Line2Polygon(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool Line2Polygon::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    geos::operation::polygonize::Polygonizer polygonizer;

    for(const SPFeatureI& infeature : _inputfeatures){
        if ( infeature->geometryType() == itLINE){
            const UPGeometry& geom = infeature->geometry();
            polygonizer.add(geom.get());
        }
    }
    std::vector<geos::geom::Polygon *> *polygons = polygonizer.getPolygons();
    for(geos::geom::Polygon *polygon : *polygons){
        _outputfeatures->newFeature(polygon);
    }
    delete polygons;

    _outputfeatures->setAttributes(_inputfeatures->attributeTable()->copyTable());

    QVariant value;
    value.setValue<IFeatureCoverage>(_outputfeatures);
    logOperation(_outputfeatures, _expression);
    ctx->setOutput(symTable, value, _outputfeatures->name(), itFEATURE,_outputfeatures->resource());



    return true;
}

Ilwis::OperationImplementation *Line2Polygon::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Line2Polygon(metaid, expr)    ;
}

Ilwis::OperationImplementation::State Line2Polygon::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);

    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputfeatures.prepare(features, itFEATURE)) {

        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }

    _outputfeatures = OperationHelperFeatures::initialize(_inputfeatures, itFEATURE, itCOORDSYSTEM | itENVELOPE|itDOMAIN);
    // auto cd = ColumnDefinition("polygons", _inputfeatures->attributeDefinitions().domain());
   //   _outputfeatures->attributeDefinitionsRef().addColumn(cd);


    return sPREPARED;
}

quint64 Line2Polygon::createMetadata()
{
    OperationResource operation({"ilwis://operations/line2polygon"});
    operation.setSyntax("line2polygon(inputfeatures)");
    operation.setLongName("Lines to Polygon");
    operation.setDescription(TR("Converts closed rings to polygons and assings the line value to the new polygon"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itFEATURE, TR("input lines"),TR("input feature coverage with lines"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPOLYGON, TR("output polygon coverage"), TR("Collection of polygons"));
    operation.setKeywords("line,polygon,vector");
    mastercatalog()->addItems({operation});
    return operation.id();
}

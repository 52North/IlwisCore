#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "geos/geom/Point.h"
#include "geos/geom/GeometryFactory.h"
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

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(Polygon2Line)

Polygon2Line::Polygon2Line()
{

}

Polygon2Line::~Polygon2Line()
{

}

Polygon2Line::Polygon2Line(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


void Polygon2Line::extractBoundary(const UPGeometry& g, quint32& record){
        const geos::geom::Geometry *geom1 = g.get();
        std::vector<geos::geom::CoordinateSequence *> coords = GeometryHelper::geometry2coords(geom1);
        geos::geom::Geometry *geometry;
        if ( coords.size() == 1)
            geometry = _outputfeatures->geomfactory()->createLineString(coords[0]);

        else if ( coords.size() > 1){
            std::vector<geos::geom::Geometry *> lines;
            for(int i=0; i < coords.size(); ++i)
                lines.push_back(_outputfeatures->geomfactory()->createLineString(coords[i]));
            geometry = _outputfeatures->geomfactory()->createMultiLineString(lines);
        }

        if ( geometry){
            SPFeatureI outfeature = _outputfeatures->newFeature(geometry);
            if (_singleId){
                outfeature->setCell(0,0);
            }else {
                outfeature->setCell(0, record);
            }
        }
}

bool Polygon2Line::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    quint32 record = 0;
    for(const auto& infeature : _inputfeatures){
        if ( infeature->geometryType() != itPOLYGON)
            continue;
        extractBoundary(infeature->geometry(),record);
        ++record;
    }
    QVariant value;
    value.setValue<IFeatureCoverage>(_outputfeatures);
    logOperation(_outputfeatures, _expression);
    ctx->setOutput(symTable, value, _outputfeatures->name(), itFEATURE,_outputfeatures->resource());

    return true;
}

Ilwis::OperationImplementation *Polygon2Line::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Polygon2Line(metaid,expr);
}

Ilwis::OperationImplementation::State Polygon2Line::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString features = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputfeatures.prepare(features, itFEATURE)) {

        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }

    _outputfeatures = OperationHelperFeatures::initialize(_inputfeatures, itFEATURE, itCOORDSYSTEM | itENVELOPE);
    std::set<QString> booleans ={ "yes","true","1"};
    auto iter = booleans.find(_expression.parm(1).value().toLower());
    _singleId = iter != booleans.end();
    if ( _singleId){
        IIndexedIdDomain dom;
        dom.prepare();
        dom->addItem(new IndexedIdentifier("boundary",0,1));
       _outputfeatures->attributeDefinitionsRef().addColumn(ColumnDefinition("boundaries", dom));
    }else {
        auto cd = ColumnDefinition("boundaries", _inputfeatures->attributeDefinitions().domain());
       _outputfeatures->attributeDefinitionsRef().addColumn(cd);
    }

    return sPREPARED;
}

quint64 Polygon2Line::createMetadata()
{
    OperationResource operation({"ilwis://operations/polygon2line"});
    operation.setSyntax("polygon2line(inputfeatures, usesingleid=true)");
    operation.setLongName("Extract polygon boundaries");
    operation.setDescription(TR("Extracts the boundaries of a polygon coverages to a line coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE, TR("input polygons"),TR("input feature coverage with polygons"));
    operation.addInParameter(1,itBOOL, TR("single id"),TR("Each line in the output map has the same id"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itLINE, TR("output line coverage"), TR("Collection of boundaries of the polygons"));
    operation.setKeywords("line,polygon,vector");
    mastercatalog()->addItems({operation});
    return operation.id();
}



#include <functional>
#include <future>
#include <memory>
#include <functional>
#include "geos/geom/Geometry.h"
#include "geos/util/GEOSException.h"
#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "geometryhelper.h"
#include "featureiterator.h"
#include "spatialrelation.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SpatialRelationOperation)

SpatialRelationOperation::SpatialRelationOperation()
{
}

SpatialRelationOperation::SpatialRelationOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *SpatialRelationOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SpatialRelationOperation(metaid, expr);
}

quint64 SpatialRelationOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/spatialrelation"});
    operation.setLongName("Spatial Relation");
    operation.setDescription("Checks if a certain boolean relation holds true for matched geometries of the input data");
    operation.setSyntax("contains(featurecoverage,geometries,contains|covers|coveredBy|touches|intersects|disjoint|within|equals|crosses|overlaps)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the contains constraint"));
    operation.setInParameterCount({3});

    operation.addInParameter(0,itFEATURE, TR("feature coverage"),TR("coverage on which the spatial relation is applicable"));
    operation.addInParameter(1,itSTRING|itFEATURE, TR("Geometries"), TR("the geometry for which the spatial relation holds true. It can come from a featurecoverage or a predefined geometry(wkt)"));
    operation.addInParameter(2,itSTRING, TR("Relation"), TR("Relation to be checked; if the relation holds true the feature will be in the output set"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("feature coverage"));
    operation.setKeywords("geometry, vector, spatial relation,features");

    mastercatalog()->addItems({operation});
    return operation.id();

}

bool SpatialRelationOperation::contains(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomCoverage->contains(geomRelation);
}
bool SpatialRelationOperation::covers(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return  geomRelation->covers(geomCoverage);
}
bool SpatialRelationOperation::coveredBy(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->coveredBy(geomCoverage);
}
bool SpatialRelationOperation::touches(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->touches(geomCoverage);
}

bool SpatialRelationOperation::intersects(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->intersects(geomCoverage);
}

bool SpatialRelationOperation::disjoint(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->disjoint(geomCoverage);
}

bool SpatialRelationOperation::within(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->within(geomCoverage);
}

bool SpatialRelationOperation::equals(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->equals(geomCoverage);
}

bool SpatialRelationOperation::crosses(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->crosses(geomCoverage);
}

bool SpatialRelationOperation::overlaps(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->overlaps(geomCoverage);
}

void SpatialRelationOperation::collectResults( const geos::geom::Geometry *geomRelation, SPFeatureI inputfeature, std::vector<SPFeatureI>& outFeatures){
    const geos::geom::Geometry *geomCoverage = inputfeature->geometry().get();
    for(int gi = 0; gi < geomRelation->getNumGeometries(); ++gi){

        if ( geomCoverage!= 0 &&_relation(geomCoverage,geomRelation->getGeometryN(gi) )){
            outFeatures.push_back(inputfeature);
        }
        int variantCount = inputfeature->subFeatureCount();
        for(int subGindex = 0; subGindex < variantCount; ++subGindex){
            const geos::geom::Geometry *geomCoverage =  inputfeature->subFeatureRef(gi)->geometry().get();

            if ( geomCoverage!= 0 &&_relation(geomCoverage,geomRelation->getGeometryN(subGindex) )){
                outFeatures.push_back(inputfeature);
            }
        }
    }
}

bool SpatialRelationOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::vector<SPFeatureI> resultset;
    int count = 0;
    geos::geom::Geometry *geomRelation = _geometry.get();
    try{
        if ( geomRelation != 0) {
            for(auto feature : _inputFeatures){
                collectResults(geomRelation, feature, resultset);
                updateTranquilizer(count++, 20);
            }
        } else {
            for(auto inputfeature : _inputFeatures){
                for(auto relatedfeatures : _relatedFeatures)    {
                    collectResults(relatedfeatures->geometry().get(), inputfeature, resultset);
                    updateTranquilizer(count++, 20);
                }
            }
        }
    } catch(geos::util::GEOSException& exc){
        ERROR0(QString(exc.what()));
        return false;
    } catch (std::bad_function_call& err){
        ERROR0(err.what());
        return false;
    }

    std::set<quint64> ids;
    for(SPFeatureI feature : resultset){
        if ( std::find(ids.begin(), ids.end(),feature->featureid()) == ids.end()) {
            ids.insert(feature->featureid());
            _outputFeatures->newFeatureFrom(feature);
        }

    }
    setOutput(_outputFeatures, ctx, symTable);

    return true;
}


OperationImplementation::State SpatialRelationOperation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString fc = _expression.parm(0).value();

    if (!_inputFeatures.prepare(fc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }
    if ( _expression.parm(1).valuetype() == itSTRING){
        QString geom = _expression.parm(1).value();
        _geometry.reset(GeometryHelper::fromWKT(geom, ICoordinateSystem())); // dont use the csy of coverage; not relevant here

        if ( !_geometry){
            ERROR2(ERR_NO_INITIALIZED_2, TR("Geometry"), TR("Contains operation"));
            return sPREPAREFAILED;
        }
        _outputFeatures = OperationHelperFeatures::initialize(_inputFeatures, itFEATURE, itCOORDSYSTEM | itENVELOPE|itDOMAIN | itTABLE);

    }else if ( hasType(_expression.parm(1).valuetype(), itFEATURE)){
        fc =  _expression.parm(1).value();
        if(!_relatedFeatures.prepare(fc)){
            ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
            return sPREPAREFAILED;
        }
        _outputFeatures = OperationHelperFeatures::initialize(_relatedFeatures, itFEATURE, itCOORDSYSTEM | itENVELOPE|itDOMAIN | itTABLE);
    }
    QString relation = _expression.parm(2).value().toLower();
    if (relation == "disjoint")
        _relation = disjoint;
    else if (relation == "contains")
        _relation = contains;
    else if (relation == "covers")
        _relation = covers;
    else if (relation == "coveredby")
        _relation = coveredBy;
   else  if (relation == "touches")
        _relation = touches;
    else if (relation == "intersects")
        _relation = intersects;
    else if (relation == "within")
        _relation = within;
    else if (relation == "crosses")
        _relation = crosses;
    else if (relation == "overlaps")
        _relation = overlaps;
    else if (relation == "equals")
        _relation = equals;
    else    {
        kernel()->issues()->log(TR("Unknown relation:") + relation);
        return sPREPAREFAILED;
    }


    initialize(_inputFeatures->featureCount());

    return sPREPARED;

}

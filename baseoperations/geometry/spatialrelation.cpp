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

SpatialRelationOperation::SpatialRelationOperation()
{
}

SpatialRelationOperation::SpatialRelationOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

quint64 SpatialRelationOperation::createMetadata(OperationResource& operation)
{
    operation.setInParameterCount({2});

    operation.addInParameter(0,itCOVERAGE, TR("coverage"),TR("coverage on which the spatial relation is applicable"));
    operation.addInParameter(1,itSTRING, TR("Geometry"), TR("the geometry for which the spatial relation holds true"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOLLECTION | itINTEGER, TR("result index collection"));
    operation.setKeywords("geometry, coordinate, coverage, spatial relation");

    mastercatalog()->addItems({operation});
    return operation.id();

}

bool SpatialRelationOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IFeatureCoverage features = _coverage.as<FeatureCoverage>();

    std::set<quint32> resultset;
    int index = 0;
    geos::geom::Geometry *geomRelation = _geometry.get();
    try{
    if ( geomRelation != 0) {
        for(auto iter : features){
                for(int gi = 0; gi < geomRelation->getNumGeometries(); ++gi){
                    const geos::geom::Geometry *geomCoverage = iter->geometry().get();

                    if ( geomCoverage!= 0 &&_relation(geomCoverage,geomRelation->getGeometryN(gi) )){
                        resultset.insert(index);
                    }
                    int variantCount = iter->subFeatureCount();
                    for(int subGindex = 0; subGindex < variantCount; ++subGindex){
                        const geos::geom::Geometry *geomCoverage =  iter->subFeatureRef(gi)->geometry().get();

                        if ( geomCoverage!= 0 &&_relation(geomCoverage,geomRelation->getGeometryN(subGindex) )){
                            resultset.insert(index);
                        }
                    }
                }
            }
            ++index;
        }
    } catch(geos::util::GEOSException& exc){
        ERROR0(QString(exc.what()));
        return false;
    }

    std::vector<quint32> result(resultset.begin(), resultset.end());
    if ( ctx != 0) {
        QVariant value;
        value.setValue<std::vector<quint32>>(result);
        ctx->setOutput(symTable, value, sUNDEF, itINTEGER | itCOLLECTION, Resource());
    }

    return true;
}


OperationImplementation::State SpatialRelationOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString fc = _expression.parm(0).value();

    if (!_coverage.prepare(fc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }

    QString quotedGeom = _expression.parm(1).value();
    QString geom = quotedGeom.remove('\"');
    _geometry.reset(GeometryHelper::fromWKT(geom, ICoordinateSystem())); // dont use the csy of coverage; not relevant here
    if ( !_geometry){
        ERROR2(ERR_NO_INITIALIZED_2, TR("Geometry"), TR("Contains operation"));
        return sPREPAREFAILED;
    }

    return sPREPARED;

}

//---------------------------------------------------------------------------------
REGISTER_OPERATION(Contains)

Contains::Contains(){}

Contains::Contains(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Contains::create(quint64 metaid, const Ilwis::OperationExpression &expr){
    return new Contains(metaid, expr);
}

bool Contains::contains(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomCoverage->contains(geomRelation);
}

OperationImplementation::State Contains::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Contains::contains;
    return sPREPARED;
}

quint64 Contains::createMetadata(){
    OperationResource operation({"ilwis://operations/contains"});
    operation.setLongName("Contains Feature Geometry");
    operation.setSyntax("contains(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the contains constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//----------------------------------------------------------------------------------
REGISTER_OPERATION(Covers)

Covers::Covers(){}

Covers::Covers(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Covers::create(quint64 metaid, const Ilwis::OperationExpression &expr){
    return new Covers(metaid, expr);
}

bool Covers::covers(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return  geomRelation->covers(geomCoverage);
}

OperationImplementation::State Covers::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Covers::covers;
    return sPREPARED;
}

quint64 Covers::createMetadata(){
    OperationResource operation({"ilwis://operations/covers"});
    operation.setLongName("Covers Feature Geometry");
    operation.setSyntax("covers(coverage,wkt-definition)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the covers constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//-----------------------------------------------------------------------------------
REGISTER_OPERATION(CoveredBy)

CoveredBy::CoveredBy(){}

CoveredBy::CoveredBy(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *CoveredBy::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CoveredBy(metaid, expr);
}

bool CoveredBy::coveredBy(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->coveredBy(geomCoverage);
}

OperationImplementation::State CoveredBy::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = CoveredBy::coveredBy;
    return sPREPARED;
}

quint64 CoveredBy::createMetadata()
{
    OperationResource operation({"ilwis://operations/coveredby"});
    operation.setLongName("Covered by Feature Geometry");
    operation.setSyntax("coveredby(coverage,wkt-definition)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the CoveredBy constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//-----------------------------------------------------------------------------------
REGISTER_OPERATION(Touches)

Touches::Touches(){}

Touches::Touches(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Touches::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Touches(metaid, expr);
}

bool Touches::touches(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->touches(geomCoverage);
}

OperationImplementation::State Touches::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Touches::touches;
    return sPREPARED;
}

quint64 Touches::createMetadata()
{
    OperationResource operation({"ilwis://operations/touches"});
    operation.setSyntax("touches(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the Touches constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//-----------------------------------------------------------------------------------
REGISTER_OPERATION(Intersects)

Intersects::Intersects(){}

Intersects::Intersects(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Intersects::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Intersects(metaid, expr);
}

bool Intersects::intersects(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->intersects(geomCoverage);
}

OperationImplementation::State Intersects::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Intersects::intersects;
    return sPREPARED;
}

quint64 Intersects::createMetadata()
{
    OperationResource operation({"ilwis://operations/intersects"});
    operation.setSyntax("intersects(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the intersects constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//--------------------------------------------------------------------------------
REGISTER_OPERATION(Disjoint)

Disjoint::Disjoint(){}

Disjoint::Disjoint(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Disjoint::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Disjoint(metaid, expr);
}

bool Disjoint::disjoint(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->disjoint(geomCoverage);
}

OperationImplementation::State Disjoint::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Disjoint::disjoint;
    return sPREPARED;
}

quint64 Disjoint::createMetadata()
{
    OperationResource operation({"ilwis://operations/disjoint"});
    operation.setSyntax("disjoint(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the disjoint constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}

//--------------------------------------------------------------------------------
REGISTER_OPERATION(Within)

Within::Within(){}

Within::Within(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Within::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Within(metaid, expr);
}

bool Within::within(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->within(geomCoverage);
}

OperationImplementation::State Within::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Within::within;
    return sPREPARED;
}

quint64 Within::createMetadata()
{
    OperationResource operation({"ilwis://operations/within"});
    operation.setSyntax("within(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the within constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}

//--------------------------------------------------------------------------------
REGISTER_OPERATION(Equals)

Equals::Equals(){}

Equals::Equals(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Equals::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Equals(metaid, expr);
}

bool Equals::equals(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->equals(geomCoverage);
}

OperationImplementation::State Equals::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Equals::equals;
    return sPREPARED;
}

quint64 Equals::createMetadata()
{
    OperationResource operation({"ilwis://operations/equals"});
    operation.setSyntax("equals(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the equals constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//--------------------------------------------------------------------------------
REGISTER_OPERATION(Crosses)

Crosses::Crosses(){}

Crosses::Crosses(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Crosses::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Crosses(metaid, expr);
}

bool Crosses::crosses(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->crosses(geomCoverage);
}

OperationImplementation::State Crosses::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Crosses::crosses;
    return sPREPARED;
}

quint64 Crosses::createMetadata()
{
    OperationResource operation({"ilwis://operations/crosses"});
    operation.setSyntax("crosses(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the Crosses constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}
//-----------------------------------------------------------------------------------
REGISTER_OPERATION(Overlaps)

Overlaps::Overlaps(){}

Overlaps::Overlaps(quint64 metaid, const Ilwis::OperationExpression &expr) : SpatialRelationOperation(metaid, expr){}

Ilwis::OperationImplementation *Overlaps::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Overlaps(metaid, expr);
}

bool Overlaps::overlaps(const geos::geom::Geometry *geomCoverage, const geos::geom::Geometry *geomRelation) {
    return geomRelation->overlaps(geomCoverage);
}

OperationImplementation::State Overlaps::prepare(ExecutionContext *ctx, const SymbolTable &sym){
    if (!SpatialRelationOperation::prepare(ctx, sym) == sPREPARED)
        return sPREPAREFAILED;
    _relation = Overlaps::overlaps;
    return sPREPARED;
}

quint64 Overlaps::createMetadata()
{
    OperationResource operation({"ilwis://operations/overlaps"});
    operation.setSyntax("overlaps(coverage,wkt-defintion)");
    operation.setDescription(TR("returns a feature-index of all features that satisfy the Overlaps constraint"));

    return SpatialRelationOperation::createMetadata(operation);
}



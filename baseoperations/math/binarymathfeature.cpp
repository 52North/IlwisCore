#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "binarymathfeature.h"

using namespace Ilwis;
using namespace BaseOperations;

BinaryMathFeature::BinaryMathFeature()
{
}

BinaryMathFeature::BinaryMathFeature(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool BinaryMathFeature::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    //IFeatureCoverage outputFC = _outputFeatures.get<FeatureCoverage>();
    IFeatureCoverage inputFC = _outputFeatures.get<FeatureCoverage>();
    FeatureIterator iterIn(inputFC);
    for_each(iterIn, iterIn.end(), [&](SPFeatureI feature){
        _outputFeatures->newFeatureFrom(feature);
    });
    return true;
}

OperationImplementation *BinaryMathFeature::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMathFeature(metaid, expr);
}

OperationImplementation::State BinaryMathFeature::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    QString featureCovName =  _expression.parm(0).value();
    if (!_inputFeatureSet1.prepare(featureCovName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,featureCovName,"" );
        return sPREPAREFAILED;
    }
    featureCovName =  _expression.parm(1).value();
    if (!_inputFeatureSet2.prepare(featureCovName)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,featureCovName,"" );
        return sPREPAREFAILED;
    }
    bool ok = false;
    if ( ctx->_masterCsy != sUNDEF) {
        ok = _csyTarget.prepare(ctx->_masterCsy);
        if (!ok){
            WARN1(WARN_COULDNT_CREATE_OBJECT_FOR_1,ctx->_masterCsy);
        }
    }
    if (!ok) {
        _csyTarget = _inputFeatureSet1->coordinateSystem();
    }

    Resource resource(_inputFeatureSet1->ilwisType() | _inputFeatureSet2->ilwisType());
    _outputFeatures.prepare(resource);
    Box2D<double> envelope = addEnvelopes();
    _outputFeatures->setCoordinateSystem(_csyTarget);
    _outputFeatures->envelope(envelope);
//    if ( _inputFeatureSet1->ilwisType() == _inputFeatureSet2->ilwisType()) {
//        Table::mergeTables(_inputFeatureSet1->attributeTable())
//    }
    QString outname = _expression.parm(0,false).value();
    if ( outname != sUNDEF)
        _outputFeatures->setName(outname);

    return sPREPARED;
}

Box3D<double> BinaryMathFeature::addEnvelopes() const {
    Box2D<double> envelope = _csyTarget->convertEnvelope(_inputFeatureSet1->coordinateSystem(),_inputFeatureSet1->envelope());
    envelope += _csyTarget->convertEnvelope(_inputFeatureSet2->coordinateSystem(),_inputFeatureSet2->envelope());
    return envelope;
}

quint64 BinaryMathFeature::createMetadata()
{

}

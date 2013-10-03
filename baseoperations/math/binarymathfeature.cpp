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
#include "tablemerger.h"
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

    FeatureIterator iterIn1(_inputFeatureSet1);
    for_each(iterIn1, iterIn1.end(), [&](SPFeatureI feature){
        _outputFeatures->newFeatureFrom(feature);
    });
    FeatureIterator iterIn2(_inputFeatureSet2);
    for_each(iterIn2, iterIn2.end(), [&](SPFeatureI feature){
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
    ITable attTable = TableMerger::mergeTables(_inputFeatureSet1->attributeTable(), _inputFeatureSet2->attributeTable(), &_renumberer);
    attTable->records(_inputFeatureSet1->featureCount() + _inputFeatureSet2->featureCount());
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
    QString url = QString("ilwis://operations/binarymathfeatures");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","binarymathfeatures");
    resource.addProperty("syntax","binarymathfeatures(featurecoverage1,featurescoverage2,add|substract)");
    resource.addProperty("description",TR("generates a new featurecoverage that puts all the features of both coverages into one coverage"));
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itFEATURE);
    resource.addProperty("pin_1_name", TR("input feature coverage"));
    resource.addProperty("pin_1_desc",TR("input feature coverage"));
    resource.addProperty("pin_2_type", itFEATURE);
    resource.addProperty("pin_2_name", TR("input feature coverage"));
    resource.addProperty("pin_2_desc",TR("input feature coverage"));
    resource.addProperty("pin_3_type", itSTRING);
    resource.addProperty("pin_3_name", TR("Operator"));
    resource.addProperty("pin_3_domain","string");
    resource.addProperty("pin_3_desc",TR("operator (add, substract) applied to the other 2 input operators"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itFEATURE);
    resource.addProperty("pout_1_name", TR("output featurecoverage"));
    resource.addProperty("pout_1_desc",TR("output feature coverage"));

    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

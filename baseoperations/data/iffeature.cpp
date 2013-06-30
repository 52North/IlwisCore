#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "commandhandler.h"
#include "featureiterator.h"
#include "ifoperation.h"
#include "iffeature.h"

using namespace Ilwis;
using namespace BaseOperations;

Ilwis::BaseOperations::IfFeature::IfFeature()
{
}

IfFeature::IfFeature(quint64 metaid, const Ilwis::OperationExpression &expr) : IfOperation(metaid, expr)
{
}

bool IfFeature::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    SubSetAsyncFunc iffunc = [&](const std::vector<quint32>& subset) -> bool {

//        FeatureIterator iterOut(_outputFC);
//        FeatureIterator iterIn(_inputFC,subset);
//        FeatureIterator iter1, iter2;
//        iter1 = iterOut;
//        bool isCoverage1 = _coverages[0].isValid();
//        bool isCoverage2 = _coverages[1].isValid();
//        if ( isCoverage1)
//            iter1 = FeatureIterator(_coverages[0].get<FeatureCoverage>(), subset);
//        if ( isCoverage2)
//            iter2 = FeatureIterator(_coverages[1].get<FeatureCoverage>(), subset);

//        while(iterIn != iterIn.end()) {
//            FeatureInterface v1,v2;
//            if ( isCoverage1) {
//                v1 = *iter1;
//                ++iter1;
//            }
//            if ( isCoverage2) {
//                v2 = *iter2;
//                ++iter2;
//            }
//            if (_number[0] != rUNDEF)
//                v1 = _number[0];
//            if ( _number[1] != rUNDEF)
//                v2 = _number[1];

//            *iterOut = *iterIn ? v1 : v2;

//            ++iterOut;
//            ++iterIn;
//        }
        return true;

    };

//    bool res = OperationHelperRaster::execute(ctx, iffunc, _outputGC);

//    if ( res && ctx != 0) {
//        QVariant value;
//        value.setValue<IFeatureCoverage>(_outputFC);
//        ctx->addOutput(symTable,value,_outputFC->name(),itGRIDCOVERAGE,_outputGC->source());
//    }
    return true;
}

Ilwis::OperationImplementation *IfFeature::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new IfFeature(metaid, expr);
}

Ilwis::OperationImplementation::State IfFeature::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString fc = _expression.parm(0).value();

    if (!_inputFC.prepare(fc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }

    OperationHelperFeatures helper;
    IIlwisObject obj = helper.initialize(_inputFC.get<IlwisObject>(), itFEATURECOVERAGE, itENVELOPE | itCOORDSYSTEM) ;
    if ( !obj.isValid()) {
        ERROR2(ERR_INVALID_INIT_FOR_2,"FeatureCoverage",fc);
        return sPREPAREFAILED;
    }
    _outputFC = obj.get<FeatureCoverage>();

    DataDefinition outputDataDef = findOutputDataDef(_expression);
    _outputFC->datadef() = outputDataDef;

    return sPREPARED;
}

quint64 IfFeature::createMetadata()
{
    QString url = QString("ilwis://operations/iff");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","iff");
    res.addProperty("syntax","iffraster(featurecoverage,outputchoicetrue, outputchoicefalse)");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itFEATURECOVERAGE);
    res.addProperty("pin_1_name", TR("input featurecoverage"));
    res.addProperty("pin_1_desc",TR("input featurecoverage with boolean domain"));
    res.addProperty("pin_2_type", itNUMERIC | itSTRING | itBOOL | itFEATURECOVERAGE);
    res.addProperty("pin_2_name", TR("true choice"));
    res.addProperty("pin_2_desc",TR("value returned when the boolean input feature is true"));
    res.addProperty("pin_3_type", itNUMERIC | itSTRING | itBOOL | itFEATURECOVERAGE);
    res.addProperty("pin_3_name", TR("false choice"));
    res.addProperty("pin_3_desc",TR("value returned when the boolean input feature is false"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itFEATURECOVERAGE);
    res.addProperty("pout_1_name", TR("featurecoverage"));
    res.addProperty("pout_1_desc",TR("featurecoverage with all features that correspond to the true value in the input having a value"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

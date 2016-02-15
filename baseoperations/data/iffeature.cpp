#include <QString>
#include <QStringList>
#include <functional>
#include <future>
#include <memory>
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
#include "featureiterator.h"
#include "ifoperation.h"
#include "iffeature.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(IfFeature)

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

        FeatureIterator iterOut(_outputFC);
        FeatureIterator iterIn(_inputFC,subset);
        FeatureIterator iterEnd = iterOut.end();
        while(iterIn != iterEnd) {
            _outputFC->newFeatureFrom(*iterIn);

            ++iterOut;
            ++iterIn;
        }
        return true;

    };

    bool resource = OperationHelperFeatures::execute(ctx, iffunc, _inputFC, _outputFC);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IFeatureCoverage>(_outputFC);
        ctx->setOutput(symTable,value,_outputFC->name(),itFEATURE,_outputFC->resource());
    }
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
    IIlwisObject obj = helper.initialize(_inputFC.as<IlwisObject>(), itFEATURE, itENVELOPE | itCOORDSYSTEM) ;
    if ( !obj.isValid()) {
        ERROR2(ERR_INVALID_INIT_FOR_2,"FeatureCoverage",fc);
        return sPREPAREFAILED;
    }
    _outputFC = obj.as<FeatureCoverage>();

    return sPREPARED;
}

quint64 IfFeature::createMetadata()
{
    QString url = QString("ilwis://operations/iff");
    Resource resource(QUrl(url), itSINGLEOPERATION);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","iff");
    resource.addProperty("syntax","iffraster(featurecoverage,outputchoicetrue, outputchoicefalse)");
    resource.addProperty("description","constructs a new coverage based on a boolean selection described by the boolean map. The true pixels are taken from the first input map, the false pixels from the second map");
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itFEATURE);
    resource.addProperty("pin_1_name", TR("input featurecoverage"));
    resource.addProperty("pin_1_desc",TR("input featurecoverage with boolean domain"));
    resource.addProperty("pin_2_type", itNUMBER | itSTRING | itBOOL | itFEATURE);
    resource.addProperty("pin_2_name", TR("true choice"));
    resource.addProperty("pin_2_desc",TR("value returned when the boolean input feature is true"));
    resource.addProperty("pin_3_type", itNUMBER | itSTRING | itBOOL | itFEATURE);
    resource.addProperty("pin_3_name", TR("false choice"));
    resource.addProperty("pin_3_desc",TR("value returned when the boolean input feature is false"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itFEATURE);
    resource.addProperty("pout_1_name", TR("featurecoverage"));
    resource.addProperty("pout_1_desc",TR("featurecoverage with all features that correspond to the true value in the input having a value"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

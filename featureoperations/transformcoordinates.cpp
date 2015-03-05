#include <functional>
#include <future>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operationhelperfeatures.h"
#include "operation.h"
#include "transformcoordinates.h"

using namespace Ilwis;
using namespace FeatureOperations;

REGISTER_OPERATION(TransformCoordinates)


TransformCoordinates::TransformCoordinates()
{
}


Ilwis::FeatureOperations::TransformCoordinates::TransformCoordinates(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}

bool Ilwis::FeatureOperations::TransformCoordinates::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    for(const auto& infeature : _inputFeatures){
        _outputFeatures->newFeatureFrom(infeature,_inputFeatures->coordinateSystem());
    }
    Table *tbl = static_cast<Table *>(_inputFeatures->attributeTable()->clone());
    ITable tblnew;
    tblnew.set(tbl);
    _outputFeatures->attributeTable(tblnew);
    int index = tblnew->columnIndex(FEATUREIDCOLUMN);
    int rec = 0;
    for(const auto& outfeature : _outputFeatures){
        tblnew->setCell(index,rec++, outfeature->featureid())    ;
    }

    if ( ctx != 0) {
        QVariant value;
        value.setValue<IFeatureCoverage>(_outputFeatures);
        ctx->setOutput(symTable, value, _outputFeatures->name(), itFEATURE,_outputFeatures->source());
    }

    return true;
}

OperationImplementation *Ilwis::FeatureOperations::TransformCoordinates::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TransformCoordinates(metaid, expr);
}

quint64 Ilwis::FeatureOperations::TransformCoordinates::createMetadata()
{
    OperationResource operation({"ilwis://operations/transformcoordinates"});
    operation.setSyntax("transformcoordinates(inputfeaturecoverage, targetcoordinatesystem)");
    operation.setDescription(TR("transforms coordinates of the geometries of the features from one coordinate system to the other(target)"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE,  TR("feature coverage"),TR("feature map whos geometries will be transformed"));
    operation.addInParameter(1,itCOORDSYSTEM, TR("target coordinate system"),TR("coordinate system of the transformed coordinates"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output feature coverage"), TR("output feature coverage with transformed coordinates"));
    operation.setKeywords("feature, coordinate transformation");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State Ilwis::FeatureOperations::TransformCoordinates::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    QString features = _expression.parm(0).value();
    QString csy = _expression.parm(1).value();

    if (!_inputFeatures.prepare(features, itFEATURE)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,features,"");
        return sPREPAREFAILED;
    }

    if (!_targetCsy.prepare(csy, itCOORDSYSTEM)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,csy,"");
        return sPREPAREFAILED;
    }
    _outputFeatures = OperationHelperFeatures::initialize(_inputFeatures,itFEATURE, itDOMAIN);
    _outputFeatures->coordinateSystem(_targetCsy);
    return sPREPARED;


}

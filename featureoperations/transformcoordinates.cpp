#include <functional>
#include <future>
#include "coverage.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "raster.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
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

TransformCoordinates::TransformCoordinates(quint64 metaid, const Ilwis::OperationExpression &expr): OperationImplementation(metaid,expr)
{

}

bool TransformCoordinates::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    return false;
}

OperationImplementation *TransformCoordinates::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TransformCoordinates(metaid, expr);
}

quint64 TransformCoordinates::createMetadata()
{
    OperationResource operation({"ilwis://operations/transformcoordinates"});
    operation.setSyntax("transformcoordinates(inputfeaturemap, csydefintion)");
    operation.setDescription(TR("transforms the coordinates of all vertices of the features from one coordinate system to another"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itFEATURE,  TR("Source feature coverage"),TR("The feature coverage to be transformed to another coordinate system"));
    operation.addInParameter(1,itSTRING, TR("coordinate system definition"),TR("defintion of new projection in terms of epsg or proj4 or the url of an existing coordinate system"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itFEATURE, TR("output feature coverage"), TR("output feature coverage where all vertices have new coordinates"));
    operation.setKeywords("features, vector, coordinatesystem,internal"); // internal for the moment as there is no implementation

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation::State TransformCoordinates::prepare(ExecutionContext *ctx, const SymbolTable &sym)
{
    OperationImplementation::prepare(ctx,sym);
    kernel()->issues()->log(TR("not implemented"));
    return sPREPAREFAILED;
}



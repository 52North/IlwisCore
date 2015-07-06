#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rastertopoint.h"

using namespace Ilwis;
using namespace RasterOperations;

REGISTER_OPERATION(RasterToPoint);

RasterToPoint::RasterToPoint()
{

}

RasterToPoint::~RasterToPoint()
{

}

RasterToPoint::RasterToPoint(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid,expr)
{

}


bool RasterToPoint::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;


    QVariant value;
    value.setValue<IFeatureCoverage>(_outputfeatures);
    ctx->setOutput(symTable,value,_outputfeatures->name(), itFEATURE, _outputfeatures->source() );
    return true;
}

Ilwis::OperationImplementation *RasterToPoint::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RasterToPoint(metaid,expr);
}

Ilwis::OperationImplementation::State RasterToPoint::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    return sPREPAREFAILED;
}

quint64 RasterToPoint::createMetadata()
{
    OperationResource operation({"ilwis://operations/raster2point"});
    operation.setLongName("Transform a rastermap to a pointmap");
    operation.setKeywords("raster,pointmap");

    /*
     * more metadata
    */

    mastercatalog()->addItems({operation});
    return operation.id();
}

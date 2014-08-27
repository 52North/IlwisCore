#include <QString>
#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "pixeliterator.h"
#include "ilwisoperation.h"
#include "ifoperation.h"
#include "iffraster.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(IffRaster)

IffRaster::IffRaster()
{
}


IffRaster::IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    IfOperation(metaid, expr)
{
    _number[0] = _number[1] = rUNDEF;
}

bool IffRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    BoxedAsyncFunc iffunc = [&](const BoundingBox& box) -> bool {

        PixelIterator iterOut(_outputGC,box);
        PixelIterator iterIn(_inputGC,box);
        PixelIterator iter1, iter2;
        bool isCoverage1 = _coverages[0].isValid();
        bool isCoverage2 = _coverages[1].isValid();
        if ( isCoverage1)
            iter1 = PixelIterator(_coverages[0].as<RasterCoverage>(), box);
        if ( isCoverage2)
            iter2 = PixelIterator(_coverages[1].as<RasterCoverage>(), box);
        PixelIterator iterEnd = iterOut.end();
        while(iterOut != iterEnd) {
            double v1,v2;
            if ( isCoverage1) {
                v1 = *iter1;
                ++iter1;
            }
            if ( isCoverage2) {
                v2 = *iter2;
                ++iter2;
            }
            if (_number[0] != rUNDEF)
                v1 = _number[0];
            if ( _number[1] != rUNDEF)
                v2 = _number[1];

            *iterOut = *iterIn ? v1 : v2;

            ++iterOut;
            ++iterIn;
        }
        return true;

    };

    bool resource = OperationHelperRaster::execute(ctx, iffunc, _outputGC);

    if ( resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        ctx->setOutput(symTable,value,_outputGC->name(),itRASTER,_outputGC->source());
    }
    return resource;
}

OperationImplementation *IffRaster::create(quint64 metaid, const OperationExpression &expr)
{
    return new IffRaster(metaid, expr);
}

OperationImplementation::State IffRaster::prepare(ExecutionContext *, const SymbolTable &)
{
    QString raster = _expression.parm(0).value();

    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    DataDefinition outputDataDef = findOutputDataDef(_expression);


    OperationHelperRaster helper;
    helper.initialize(_inputGC, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    _outputGC->datadefRef() = outputDataDef;

    return sPREPARED;
}



quint64 IffRaster::createMetadata()
{
    QString url = QString("ilwis://operations/iff");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","iff");
    resource.addProperty("syntax","iffraster(rastercoverage,outputchoicetrue, outputchoicefalse)");
    resource.addProperty("description","constructs a new coverage based on a boolean selection described by the boolean map. The true pixels are taken from the first input map, the false pixels from the second map");
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("input rastercoverage"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with boolean domain"));
    resource.addProperty("pin_2_type", itNUMBER | itSTRING | itBOOL | itRASTER);
    resource.addProperty("pin_2_name", TR("true choice"));
    resource.addProperty("pin_2_desc",TR("value returned when the boolean input pixel is true"));
    resource.addProperty("pin_3_type", itNUMBER | itSTRING | itBOOL | itRASTER);
    resource.addProperty("pin_3_name", TR("false choice"));
    resource.addProperty("pin_3_desc",TR("value returned when the boolean input pixel is false"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("rastercoverage"));
    resource.addProperty("pout_1_desc",TR("rastercoverage with all pixels that correspond to the true value in the input having a value"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

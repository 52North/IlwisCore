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

    BoxedAsyncFunc iffunc = [&](const Box3D<qint32>& box) -> bool {

        PixelIterator iterOut(_outputGC,box);
        PixelIterator iterIn(_inputGC,box);
        PixelIterator iter1, iter2;
        bool isCoverage1 = _coverages[0].isValid();
        bool isCoverage2 = _coverages[1].isValid();
        if ( isCoverage1)
            iter1 = PixelIterator(_coverages[0].get<GridCoverage>(), box);
        if ( isCoverage2)
            iter2 = PixelIterator(_coverages[1].get<GridCoverage>(), box);

        while(iterOut != iterOut.end()) {
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

    bool res = OperationHelperRaster::execute(ctx, iffunc, _outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);
        ctx->addOutput(symTable,value,_outputGC->name(),itGRID,_outputGC->resource());
    }
    return res;
}

OperationImplementation *IffRaster::create(quint64 metaid, const OperationExpression &expr)
{
    return new IffRaster(metaid, expr);
}

OperationImplementation::State IffRaster::prepare(ExecutionContext *, const SymbolTable &)
{
    QString gc = _expression.parm(0).value();

    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    DataDefinition outputDataDef = findOutputDataDef(_expression);


    OperationHelperRaster helper;
    helper.initialize(_inputGC, _outputGC, _expression.parm(0),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    _outputGC->datadef() = outputDataDef;

    return sPREPARED;
}



quint64 IffRaster::createMetadata()
{
    QString url = QString("ilwis://operations/iff");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","iff");
    res.addProperty("syntax","iffraster(gridcoverage,outputchoicetrue, outputchoicefalse)");
    res.addProperty("description","constructs a new coverage based on a boolean selection described by the boolean map. The true pixels are taken from the first input map, the false pixels from the second map");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itGRID);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with boolean domain"));
    res.addProperty("pin_2_type", itNUMERIC | itSTRING | itBOOL | itGRID);
    res.addProperty("pin_2_name", TR("true choice"));
    res.addProperty("pin_2_desc",TR("value returned when the boolean input pixel is true"));
    res.addProperty("pin_3_type", itNUMERIC | itSTRING | itBOOL | itGRID);
    res.addProperty("pin_3_name", TR("false choice"));
    res.addProperty("pin_3_desc",TR("value returned when the boolean input pixel is false"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRID);
    res.addProperty("pout_1_name", TR("gridcoverage"));
    res.addProperty("pout_1_desc",TR("gridcoverage with all pixels that correspond to the true value in the input having a value"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

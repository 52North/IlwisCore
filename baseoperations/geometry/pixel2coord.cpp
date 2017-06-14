#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixel2coord.h"

using namespace Ilwis;

REGISTER_OPERATION(Pixel2Coord)

Pixel2Coord::Pixel2Coord()
{
}

Pixel2Coord::Pixel2Coord(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Pixel2Coord::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    Coordinate crd = _inputGC->georeference()->pixel2Coord(Pixeld(_pixel.x, _pixel.y));
    QVariant var;
    var.setValue<Coordinate>(crd);
    ctx->setOutput(symTable,var,_outName,itCOORDINATE, Resource());

    return true;
}

OperationImplementation *Pixel2Coord::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Pixel2Coord(metaid, expr);
}

Ilwis::OperationImplementation::State Pixel2Coord::prepare(ExecutionContext *ctx, const SymbolTable& st)
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 2) {
        QString name = _expression.parm(1).value();
        QVariant var = st.getValue(name);
        _pixel = var.value<Pixel>();
    }
    if ( _expression.parameterCount() == 3) {
        bool ok1, ok2, ok3=true;
        qint32 x = _expression.parm(1).value().toInt(&ok1);
        qint32 y = _expression.parm(2).value().toInt(&ok2);
        qint32 z = 0;
        if(_expression.parameterCount() == 4) {
            z  = _expression.parm(2).value().toInt(&ok3);
        }
        if (! (ok1 && ok2 && ok3)) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"Pixel", QString("%1 %2 %3").arg(x,y,x));
            return sPREPAREFAILED;
        }
        _pixel = Pixel(x,y,z)    ;
    }
    if ( _expression.parameterCount(false) == 1) {
        _outName = _expression.parm(0,true,false).value();
    }
    if ( _pixel.isValid())
        return sPREPARED;

    return sPREPAREFAILED;
}

quint64 Pixel2Coord::createMetadata()
{
    OperationResource operation({"ilwis://operations/pixel2coord"});
    operation.setLongName("Pixel to Coordinate");
    operation.setSyntax("pixel2coord(datatype=rastercoverage|georef,Pixel)");
    operation.setDescription(TR("translates a pixel to a coordinate location based on the supplied georeference"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER | itGEOREF, TR("input rastercoverage or georeference"),TR("input rastercoverage with domain any domain or georefence"));
    operation.addInParameter(1,itPIXEL, TR("source pixel"),TR("the pixel that has to translted to a pixel location"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOORDINATE, TR("Coordinate"));
    operation.setKeywords("pixel, coordinate, geometry, transformation,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "coord2pixel.h"

using namespace Ilwis;

REGISTER_OPERATION(Coord2Pixel)

Coord2Pixel::Coord2Pixel()
{
}

Coord2Pixel::Coord2Pixel(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Coord2Pixel::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    Pixel pix = _inputGC->georeference()->coord2Pixel(_coord);
    QVariant var;
    var.setValue<Pixel>(pix);
    ctx->setOutput(symTable,var,_outName,itPIXEL, Resource());

    return true;
}

OperationImplementation *Coord2Pixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Coord2Pixel(metaid, expr);
}

Ilwis::OperationImplementation::State Coord2Pixel::prepare(ExecutionContext *ctx, const SymbolTable& st)
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
        _coord = var.value<Coordinate>();
    }
    if ( _expression.parameterCount() == 3) {
        bool ok1, ok2, ok3=true;
        double x = _expression.parm(1).value().toDouble(&ok1);
        double y = _expression.parm(2).value().toDouble(&ok2);
        double z = 0;
        if(_expression.parameterCount() == 4) {
            z  = _expression.parm(2).value().toDouble(&ok3);
        }
        if (! (ok1 && ok2 && ok3)) {
            ERROR2(ERR_ILLEGAL_VALUE_2,"Coordinate", QString("%1 %2 %3").arg(x,y,x));
            return sPREPAREFAILED;
        }
        _coord = Coordinate(x,y,z)    ;
    }
    if ( _expression.parameterCount(false) == 1) {
        _outName = _expression.parm(0,true,false).value();
    }
    if ( _coord.isValid())
        return sPREPARED;

    return sPREPAREFAILED;
}

quint64 Coord2Pixel::createMetadata()
{
    OperationResource operation({"ilwis://operations/coord2pixel"});
    operation.setSyntax("coord2pixel(datatype=rastercoverage|georef,Coordinate)");
    operation.setLongName("Coordinate to Pixel");
    operation.setDescription(TR("translates a coordinate to a pixel location based on the supplied georeference"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itRASTER | itGEOREF, TR("input rastercoverage or georeference"),TR("input rastercoverage with domain any domain or georefence"));
    operation.addInParameter(1,itCOORDINATE, TR("source coordinate"),TR("the coordinate that has to translted to a pixel location"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itPIXEL, TR("Pixel"));
    operation.setKeywords("pixel, coordinate, geometry,transformation,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}

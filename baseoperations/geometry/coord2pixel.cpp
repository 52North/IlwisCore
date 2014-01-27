#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "coord2pixel.h"

using namespace Ilwis;

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

Ilwis::OperationImplementation::State Coord2Pixel::prepare(ExecutionContext *ctx, const SymbolTable& symTable)
{
    QString raster = _expression.parm(0).value();
    if (!_inputGC.prepare(raster)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    if ( _expression.parameterCount() == 2) {
        QString name = _expression.parm(1).value();
        QVariant var = symTable.getValue(name);
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
    QString url = QString("ilwis://operations/coord2pixel");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","coord2pixel");
    resource.addProperty("syntax","Coord2Pixel(rastercoverage|georef,Coordinate)");
    resource.addProperty("description",TR("translates a coordinate to a pixel location based on the supplied georeference"));
    resource.addProperty("inparameters","2");
    resource.addProperty("pin_1_type", itRASTER | itGEOREF);
    resource.addProperty("pin_1_name", TR("input rastercoverage or georeference"));
    resource.addProperty("pin_1_desc",TR("input rastercoverage with domain any domain or georefence"));
    resource.addProperty("pin_2_type", itCOORDINATE);
    resource.addProperty("pin_2_name", TR("source pixel"));
    resource.addProperty("pin_2_desc",TR("the coordinate that has to translted to a pixel location"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itPIXEL);
    resource.addProperty("pout_1_name", TR("Pixel"));
    resource.addProperty("pout_1_desc",TR("Pixel"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

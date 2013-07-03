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
    ctx->addOutput(symTable,var,_outName,itPIXEL, Resource());

    return true;
}

OperationImplementation *Coord2Pixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Coord2Pixel(metaid, expr);
}

Ilwis::OperationImplementation::State Coord2Pixel::prepare(ExecutionContext *ctx, const SymbolTable& symTable)
{
    QString gc = _expression.parm(0).value();
    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
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
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","coord2pixel");
    res.addProperty("syntax","Coord2Pixel(gridcoverage|georef,Coordinate)");
    res.addProperty("inparameters","2");
    res.addProperty("pin_1_type", itGRID | itGEOREF);
    res.addProperty("pin_1_name", TR("input gridcoverage or georeference"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with domain any domain or georefence"));
    res.addProperty("pin_2_type", itCOORD3D);
    res.addProperty("pin_2_name", TR("source pixel"));
    res.addProperty("pin_2_desc",TR("the coordinate that has to translted to a pixel location"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itPIXEL);
    res.addProperty("pout_1_name", TR("Pixel"));
    res.addProperty("pout_1_desc",TR("Pixel"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "fcoordinate.h"

using namespace Ilwis;
using namespace BaseOperations;

FCoordinate::FCoordinate()
{
}

FCoordinate::FCoordinate(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool FCoordinate::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;
    if ( !_coord.isValid())
        return false;

    QVariant value;
    value.setValue<Coordinate>(_coord);
    ctx->_results.push_back(value);

    return true;
}

Ilwis::OperationImplementation *FCoordinate::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FCoordinate(metaid, expr);
}

Ilwis::OperationImplementation::State FCoordinate::prepare(ExecutionContext *)
{
    bool ok;
    double sx = _expression.parm(0).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(0).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        double sz = _expression.parm(0).value().toDouble(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
            return sPREPAREFAILED;
        }
    }
    _coord = Coordinate(sx,sy,sz);
}

quint64 FCoordinate::createMetadata()
{

    QString url = QString("ilwis://operations/coordinate");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","coordinate");
    res.addProperty("syntax","coordinate(x,y[,z])");
    res.addProperty("inparameters","2|3");
    res.addProperty("pin_1_type", itNUMERIC);
    res.addProperty("pin_1_name", TR("x"));
    res.addProperty("pin_1_desc",TR("x part of coordinate"));
    res.addProperty("pin_2_type", itNUMERIC);
    res.addProperty("pin_2_name", TR("y"));
    res.addProperty("pin_2_desc",TR("y part of coordinate"));
    res.addProperty("pin_3_type", itNUMERIC);
    res.addProperty("pin_3_name", TR("z"));
    res.addProperty("pin_3_desc",TR("z part of coordinate, optional"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itCOORDINATE);
    res.addProperty("pout_1_name", TR("coordinate"));
    res.addProperty("pout_1_desc",TR("coordinate"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


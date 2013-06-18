#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "FPixel.h"

using namespace Ilwis;
using namespace BaseOperations;

FPixel::FPixel()
{
}

FPixel::FPixel(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool FPixel::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;
    if ( !_voxel.isValid())
        return false;

    QVariant value;
    value.setValue<Voxel>(_voxel);
    ctx->addOutput(symTable, value, sUNDEF, itVOXEL);
    return true;
}

Ilwis::OperationImplementation *FPixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FPixel(metaid, expr);
}

Ilwis::OperationImplementation::State FPixel::prepare(ExecutionContext *)
{
    bool ok;
    double sx = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"voxel","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"voxel","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        sz = _expression.parm(0).value().toLong(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"voxel","x");
            return sPREPAREFAILED;
        }
    }
    _voxel = Voxel(sx,sy,sz);

    return sPREPARED;
}

quint64 FPixel::createMetadata()
{

    QString url = QString("ilwis://operations/pixel");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","pixel");
    res.addProperty("syntax","pixel(x,y[,z])");
    res.addProperty("inparameters","2|3");
    res.addProperty("pin_1_type", itNUMERIC);
    res.addProperty("pin_1_name", TR("x"));
    res.addProperty("pin_1_desc",TR("x part of pixel"));
    res.addProperty("pin_2_type", itNUMERIC);
    res.addProperty("pin_2_name", TR("y"));
    res.addProperty("pin_2_desc",TR("y part of pixel"));
    res.addProperty("pin_3_type", itNUMERIC);
    res.addProperty("pin_3_name", TR("z"));
    res.addProperty("pin_3_desc",TR("z part of pixel, optional"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itCOORDINATE);
    res.addProperty("pout_1_name", TR("pixel"));
    res.addProperty("pout_1_desc",TR("pixel"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}


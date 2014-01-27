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
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    if ( !_pixel.isValid())
        return false;

    QVariant value;
    value.setValue<Pixel>(_pixel);
    ctx->setOutput(symTable, value, sUNDEF, itPIXEL, Resource());
    return true;
}

Ilwis::OperationImplementation *FPixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FPixel(metaid, expr);
}

Ilwis::OperationImplementation::State FPixel::prepare(ExecutionContext *, const SymbolTable&)
{
    bool ok;
    double sx = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(0).value().toLong(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        sz = _expression.parm(0).value().toLong(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"pixel","x");
            return sPREPAREFAILED;
        }
    }
    _pixel = Pixel(sx,sy,sz);

    return sPREPARED;
}

quint64 FPixel::createMetadata()
{

    QString url = QString("ilwis://operations/pixel");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","pixel");
    resource.addProperty("syntax","pixel(x,y[,z])");
    resource.addProperty("description",TR("translates a x and y value (optional z) into a single pixel object"));
    resource.addProperty("inparameters","2|3");
    resource.addProperty("pin_1_type", itNUMBER);
    resource.addProperty("pin_1_name", TR("x"));
    resource.addProperty("pin_1_desc",TR("x part of pixel"));
    resource.addProperty("pin_2_type", itNUMBER);
    resource.addProperty("pin_2_name", TR("y"));
    resource.addProperty("pin_2_desc",TR("y part of pixel"));
    resource.addProperty("pin_3_type", itNUMBER);
    resource.addProperty("pin_3_name", TR("z"));
    resource.addProperty("pin_3_desc",TR("z part of pixel, optional"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itCOORDINATE);
    resource.addProperty("pout_1_name", TR("pixel"));
    resource.addProperty("pout_1_desc",TR("pixel"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}


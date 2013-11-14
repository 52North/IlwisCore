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
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    if ( !_coord.isValid())
        return false;

    QVariant value;
    value.setValue<Voxel>(_coord);
    ctx->addOutput(symTable, value, sUNDEF, itCOORD3D, Resource());

    return true;
}

Ilwis::OperationImplementation *FCoordinate::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new FCoordinate(metaid, expr);
}

Ilwis::OperationImplementation::State FCoordinate::prepare(ExecutionContext *, const SymbolTable &)
{
    bool ok;
    double sx = _expression.parm(0).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
        return sPREPAREFAILED;
    }
    double sy = _expression.parm(1).value().toDouble(&ok);
    if (!ok){
        ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","y");
        return sPREPAREFAILED;
    }
    double sz = 0;
    if (_expression.parameterCount() == 3) {
        sz = _expression.parm(0).value().toDouble(&ok);
        if (!ok){
            ERROR2(ERR_ILLEGAL_VALUE_2,"coordinate","x");
            return sPREPAREFAILED;
        }
    }
    _coord = Coordinate(sx,sy,sz);

    return sPREPARED;
}

quint64 FCoordinate::createMetadata()
{

    QString url = QString("ilwis://operations/coordinate");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","coordinate");
    resource.addProperty("syntax","coordinate(x,y[,z])");
    resource.addProperty("description",TR("translates a x and y value (optional z) into a single coordinate object"));
    resource.addProperty("inparameters","2|3");
    resource.addProperty("pin_1_type", itNUMBER);
    resource.addProperty("pin_1_name", TR("x"));
    resource.addProperty("pin_1_desc",TR("x part of coordinate"));
    resource.addProperty("pin_2_type", itNUMBER);
    resource.addProperty("pin_2_name", TR("y"));
    resource.addProperty("pin_2_desc",TR("y part of coordinate"));
    resource.addProperty("pin_3_type", itNUMBER);
    resource.addProperty("pin_3_name", TR("z"));
    resource.addProperty("pin_3_desc",TR("z part of coordinate, optional"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itCOORDINATE);
    resource.addProperty("pout_1_name", TR("coordinate"));
    resource.addProperty("pout_1_desc",TR("coordinate"));
    resource.addProperty("status","internal");
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}


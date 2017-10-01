#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "boundsonlycoordinatesystem.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "createboundsonlycsy.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateBoundsOnlyCsy)

CreateBoundsOnlyCsy::CreateBoundsOnlyCsy()
{

}

CreateBoundsOnlyCsy::CreateBoundsOnlyCsy(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateBoundsOnlyCsy::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IBoundsOnlyCoordinateSystem csy;
    csy->envelope(Envelope(Coordinate(_minx, _miny), Coordinate(_maxx, _maxy)));
    csy->setDescription(_description);
    QVariant value;
    value.setValue<IBoundsOnlyCoordinateSystem>(csy);
    logOperation(csy, _expression);
    ctx->setOutput(symTable,value,csy->name(),itCOORDSYSTEM,csy->resource());

    return true;
}

Ilwis::OperationImplementation *CreateBoundsOnlyCsy::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateBoundsOnlyCsy(metaid, expr);
}

Ilwis::OperationImplementation::State CreateBoundsOnlyCsy::prepare(ExecutionContext *ctx, const SymbolTable &symbolTable)
{

    bool ok;
    OperationImplementation::prepare(ctx,symbolTable);
    _minx = _expression.input<double>(0,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid minx value").arg(_expression.parm(0).value())));
        return sPREPAREFAILED;
    }
    _miny = _expression.input<double>(1,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid miny value").arg(_expression.parm(1).value())));
        return sPREPAREFAILED;
    }
    _maxx = _expression.input<double>(2,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid maxx value").arg(_expression.parm(2).value())));
        return sPREPAREFAILED;
    }
    _maxy = _expression.input<double>(3,ok);
    if (!ok){
        kernel()->issues()->log(QString(TR("%1 is not a valid maxy value").arg(_expression.parm(3).value())));
        return sPREPAREFAILED;
    }

    _description = _expression.input<QString>(4);
    QString outputName = _expression.parm(0,false).value();
    if ( outputName != sUNDEF)
        _name = outputName;
    return sPREPARED;
}

quint64 CreateBoundsOnlyCsy::createMetadata()
{
    OperationResource resource({"ilwis://operations/createboundsonlycsy"});
    resource.setLongName("Create Bounds-only Coordinatesystem");
    resource.setSyntax("createcoundsonlycsy(minx, miny, maxx, maxy)");
    resource.setInParameterCount({5});
    resource.addInParameter(0, itDOUBLE,TR("Min x"), TR("Minimum X coordinate of the corner of the boundign box"));
    resource.addInParameter(1, itDOUBLE,TR("Min y"), TR("Minimum Y coordinate of the corner of the boundign box"));
    resource.addInParameter(2, itDOUBLE,TR("Max x"), TR("Maximum X coordinate of the corner of the boundign box"));
    resource.addInParameter(3, itDOUBLE,TR("Max y"), TR("Minimum T coordinate of the corner of the boundign box"));
    resource.addInParameter(4, itSTRING,TR("Description"), TR("Extra information about the new coordinatesystem"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itGEOREF, TR("output coordinatesystem"), TR("The newly created coordinatesystem"));
    resource.setKeywords("coordinatesystem, create, geometry,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}




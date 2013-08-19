#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "rasvalue.h"

using namespace Ilwis;
using namespace BaseOperations;

RasValue::RasValue()
{
}

RasValue::RasValue(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool RasValue::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    double v = _inputGC->pix2value(_vox);
    if ( ctx) {
        ctx->addOutput(symTable, QVariant(v), sUNDEF, itDOUBLE, Resource());
    }
    return true;
}

Ilwis::OperationImplementation *RasValue::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new RasValue(metaid, expr);
}

Ilwis::OperationImplementation::State RasValue::prepare(ExecutionContext *, const SymbolTable&)
{
    if ( _expression.parameterCount() < 3 || _expression.parameterCount() > 4) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","3 or 4",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    QString gc = _expression.parm(0).value();
    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    _vox = Voxel(_expression.parm(1).value().toInt(), _expression.parm(2).value().toInt());
    if ( _expression.parameterCount() == 4)
        _vox.z(_expression.parm(3).value().toInt());
    if (!_vox.isValid()) {
        ERROR2(ERR_INVALID_PROPERTY_FOR_2,"Pixel coordinate","rasvalue");
        return sPREPAREFAILED;
    }
    return sPREPARED;
}

quint64 RasValue::createMetadata()
{
    QString url = QString("ilwis://operations/gridvalue");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","gridvalue");
    res.addProperty("syntax","rasvalue(inputgridcoverage,x,y,[,z])");
    res.addProperty("description",TR("returns the value at pixel location x,y (optional z)"));
    res.addProperty("inparameters","3|4");
    res.addProperty("pin_1_type", itGRID);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with any domain"));
    res.addProperty("pin_2_type", itINTEGER);
    res.addProperty("pin_2_name", TR("column"));
    res.addProperty("pin_2_desc",TR("column number in the grid"));
    res.addProperty("pin_3_type", itINTEGER);
    res.addProperty("pin_3_name", TR("row"));
    res.addProperty("pin_3_desc",TR("row number in the grid"));
    res.addProperty("pin_4_type", itINTEGER);
    res.addProperty("pin_4_name", TR("band number"));
    res.addProperty("pin_4_desc",TR("band number in the grid, optional"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itDOUBLE);
    res.addProperty("pout_1_name", TR("value at the indicated location"));
    res.addProperty("pout_1_desc",TR(""));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

#include <QString>
#include <functional>
#include <future>
#include "raster.h"
#include "ilwisoperation.h"
#include "iffraster.h"

using namespace Ilwis;
using namespace BaseOperations;

IffRaster::IffRaster()
{
}


IffRaster::IffRaster(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool IffRaster::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    return false;
}

OperationImplementation *IffRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new IffRaster(metaid, expr);
}

OperationImplementation::State IffRaster::prepare()
{
    QString gc = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();

    if (!_inputGC.prepare(gc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
        return sPREPAREFAILED;
    }
    const Parameter& choice1 = _expression.parm(1);
    QString outputchoice1 = choice1.value();
    Resource map1 = mastercatalog()->name2Resource(outputchoice1, itGRIDCOVERAGE);
    if ( !map1.isValid() && choice1.domain() != sUNDEF) {
        IDomain dm;
        dm.prepare(choice1.domain());
        if ( dm->contains(choice1.value()) != Domain::cNONE) {

        }

    }

    return sNOTPREPARED;
}

quint64 IffRaster::createMetadata()
{
    QString url = QString("ilwis://operations/iffraster");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","iffraster");
    res.addProperty("syntax","iffraster(gridcoverage,outputchoicetrue, outputchoicefalse)");
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with boolean domain"));
    res.addProperty("pin_2_type", itNUMERIC | itSTRING | itBOOL | itGRIDCOVERAGE);
    res.addProperty("pin_2_name", TR("true choice"));
    res.addProperty("pin_2_desc",TR("value returned when the boolean input pixel is true"));
    res.addProperty("pin_3_type", itNUMERIC | itSTRING | itBOOL | itGRIDCOVERAGE);
    res.addProperty("pin_3_name", TR("false choice"));
    res.addProperty("pin_3_desc",TR("value returned when the boolean input pixel is false"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("gridcoverage"));
    res.addProperty("pout_1_desc",TR("gridcoverage with all pixels that correspond to the true value in the input having a value"));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

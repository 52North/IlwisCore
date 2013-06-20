#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "mastercatalog.h"
#include "assignment.h"

using namespace Ilwis;
using namespace BaseOperations;

Assignment::Assignment()
{
}


Assignment::Assignment(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

bool Assignment::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::function<bool(const Box3D<qint32>)> Assign = [&](const Box3D<qint32> box ) -> bool {
        PixelIterator iterIn(_inputGC, box);
        PixelIterator iterOut(_outputGC, box);

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in = *iterIn;
            if ( v_in != rUNDEF) {
                v = v_in;
            }
            ++iterIn;
            ++iterOut;
        });
        return true;
    };

    bool res = OperationHelper::execute(Assign, _outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);
        ctx->_results.push_back(_outputGC->name());
        symTable.addSymbol(_outputGC->name(),0, itGRIDCOVERAGE,value);
    }
    return res;
}

Ilwis::OperationImplementation *Assignment::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Assignment(metaid, expr);
}

Ilwis::OperationImplementation::State Assignment::prepare(ExecutionContext *, const SymbolTable &)
{
    if ( _expression.parameterCount() != 1) {
        ERROR3(ERR_ILLEGAL_NUM_PARM3,"rasvalue","1",QString::number(_expression.parameterCount()));
        return sPREPAREFAILED;
    }

    QString coverage = _expression.parm(0).value();
    Resource res = mastercatalog()->name2Resource(coverage);
    if ( !res.isValid()) {
        ERROR1(ERR_COULD_NOT_OPEN_READING_1,coverage);
        return sPREPAREFAILED;
    }
//    if ( res.ilwisType() == itGRIDCOVERAGE) {
//        if (!_inputGC.prepare(coverage)) {
//            ERROR2(ERR_COULD_NOT_LOAD_2,coverage,"");
//            return sPREPAREFAILED;
//        }
//    } else if ( res.ilwisType() == itPOLYGONCOVERAGE) {
//        IIlwisObject ob;
//        ob.prepare(coverage, res.ilwisType());
//    }
    IIlwisObject objIn;
    objIn.prepare(coverage, res.ilwisType());
    OperationHelper helper;
    helper.initialize(objIn, res.ilwisType(), _expression.parm(0),
                                itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF | itDOMAIN | itTABLE);
    return sPREPARED;
}

quint64 Assignment::createMetadata()
{
    QString url = QString("ilwis://operations/assignment");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","assignment");
    res.addProperty("syntax","assignment(gridcoverage)");
    res.addProperty("inparameters","1");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_desc",TR("input gridcoverage with any domain"));
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("copied object"));
    res.addProperty("pout_1_desc",TR(""));
    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();

}

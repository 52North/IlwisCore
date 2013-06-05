#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "ilwisoperation.h"
#include "pixeliterator.h"
#include "unarymath.h"

using namespace Ilwis;
using namespace BaseOperations;

UnaryMath::UnaryMath() {

}

UnaryMath::UnaryMath(quint64 metaid,const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr),_spatialCase(true), _number(rUNDEF)
{

}

bool UnaryMath::execute(UnaryFunction fun, ExecutionContext *ctx)
{
    BoxedAsyncFunc unaryFun = [&](const Box3D<qint32>& box) -> bool {
        PixelIterator iterIn(_inputGC, _box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(_box.size()));

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            if ( (v_in = *iterIn) != rUNDEF) {
                v = fun(v_in);
            }
            ++iterIn;
        });
        return true;
    };

    bool res = OperationHelper::execute(unaryFun, _outputGC);

    if ( res && ctx != 0) {
        QVariant value;
        value.setValue<IGridCoverage>(_outputGC);

        ctx->_results.push_back(value);

        return true;
    }
    return false;
}

OperationImplementation::State UnaryMath::prepare()
{
    IlwisTypes ptype = _expression.parm(0).valuetype();


    if ( ptype & itNUMERIC ) {
        _spatialCase = false;
        bool ok;
        _number = _expression.parm(0).value().toDouble(&ok);
        if (!ok) {
            ERROR2(ERR_NO_OBJECT_TYPE_FOR_2,"Numerical value", "UnaryMath operation");
            _number = rUNDEF;
            return sPREPAREFAILED;
        }
        return sPREPARED;

    } else if ( ptype == itGRIDCOVERAGE) {
        QString gc = _expression.parm(0).value();
        QString outputName = _expression.parm(0,false).value();
        if ( outputName != sUNDEF)
            _outputGC->setName(outputName);

        if (!_inputGC.prepare(gc)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,gc,"");
            return sPREPAREFAILED;
        }
        OperationHelper helper;
        _box = helper.initialize(_inputGC, _outputGC, _expression.parm(0),
                                    itGRIDSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
        if ( !_outputGC.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output gridcoverage");
            return sPREPAREFAILED;
        }

        _spatialCase = true;
        return sPREPARED;
    }
    return sNOTPREPARED;
}

void UnaryMath::populateMetadata(Resource& res) {

    res.addProperty("namespace","ilwis");
    res.addProperty("inparameters","1");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.prepare();
    QString url = res.url().toString();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

}



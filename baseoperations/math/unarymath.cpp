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

UnaryMath::UnaryMath(quint64 metaid, const Ilwis::OperationExpression& expr, const QString &outpDom, UnaryFunction fun) :
    OperationImplementation(metaid, expr),
    _spatialCase(true),
    _number(rUNDEF),
    _outputDomain(outpDom),
    _unaryFun(fun)
{

}

bool UnaryMath::execute(ExecutionContext *ctx)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare()) != sPREPARED)
            return false;

    QVariant value;
    if ( _spatialCase) {
        BoxedAsyncFunc unaryFun = [&](const Box3D<qint32>& box) -> bool {
            PixelIterator iterIn(_inputGC, _box);
            PixelIterator iterOut(_outputGC, Box3D<qint32>(_box.size()));

            double v_in = 0;
            for_each(iterOut, iterOut.end(), [&](double& v){
                if ( (v_in = *iterIn) != rUNDEF) {
                    v = _unaryFun(v_in);
                }
                ++iterIn;
            });
            return true;
        };

        bool res = OperationHelper::execute(unaryFun, _outputGC);

        if ( res && ctx != 0) {
            value.setValue<IGridCoverage>(_outputGC);
        }
    } else {
        double v = sin(_number);
        value.setValue<double>(v);
    }
    if ( ctx && value.isValid())
        ctx->_results.push_back(value);

    return true;
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
        QString outputName = _expression.parm(0,false).value();
        if ( outputName != sUNDEF)
            _outputGC->setName(outputName);

        IDomain dom;
        if(!dom.prepare(_outputDomain))
            return sPREPAREFAILED;

        _outputGC->datadef().domain(dom);
        _unaryFun = sin;

        _spatialCase = true;
        return sPREPARED;
    }
    return sNOTPREPARED;
}

Resource UnaryMath::populateMetadata(const QString& item, const QString& longname, const QString& outputDom) {
    Resource res(QUrl(item), itOPERATIONMETADATA);
    int index = item.lastIndexOf("/");
    QString name = item.mid(index);
    res.addProperty("syntax", QString("%1(gridcoverage|double)").arg(name));
    res.addProperty("longname",longname);
    res.addProperty("namespace","ilwis");
    res.addProperty("inparameters","1");
    res.addProperty("pin_1_type", itGRIDCOVERAGE);
    res.addProperty("pin_1_name", TR("input gridcoverage"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_type", itGRIDCOVERAGE);
    res.addProperty("pout_1_domain",outputDom);
    res.prepare();
    QString url = res.url().toString();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    return res;

}



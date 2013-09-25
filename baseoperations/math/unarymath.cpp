#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
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

bool UnaryMath::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

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

        bool resource = OperationHelperRaster::execute(ctx, unaryFun, _outputGC);

        if ( resource && ctx != 0) {
            QVariant value;
            value.setValue<IRasterCoverage>(_outputGC);
            ctx->addOutput(symTable,value,_outputGC->name(), itRASTER,_outputGC->source() );
        }
    } else {
        double v = _unaryFun(_number);
        ctx->addOutput(symTable, QVariant(v), sUNDEF, itDOUBLE, Resource());

    }


    return true;
}

OperationImplementation::State UnaryMath::prepare(ExecutionContext *,const SymbolTable&)
{
    IlwisTypes ptype = _expression.parm(0).valuetype();


    if ( ptype & itNUMBER ) {
        _spatialCase = false;
        bool ok;
        _number = _expression.parm(0).value().toDouble(&ok);
        if (!ok) {
            ERROR2(ERR_NO_OBJECT_TYPE_FOR_2,"Numerical value", "UnaryMath operation");
            _number = rUNDEF;
            return sPREPAREFAILED;
        }
        return sPREPARED;

    } else if ( ptype == itRASTER) {
        QString raster = _expression.parm(0).value();

        if (!_inputGC.prepare(raster)) {
            ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
            return sPREPAREFAILED;
        }
        OperationHelperRaster helper;
        _box = helper.initialize(_inputGC, _outputGC, _expression.parm(0),
                                    itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);
        if ( !_outputGC.isValid()) {
            ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
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
    Resource resource(QUrl(item), itOPERATIONMETADATA);
    int index = item.lastIndexOf("/");
    QString name = item.mid(index);
    resource.addProperty("syntax", QString("%1(rastercoverage|double)").arg(name));
    resource.addProperty("description",TR("generates a new numrical rastercoverage/featurecoverage based on the operation, applied to all the pixels"));
    resource.addProperty("longname",longname);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("inparameters","1");
    resource.addProperty("pin_1_type", itRASTER);
    resource.addProperty("pin_1_name", TR("input rastercoverage"));
    resource.addProperty("pin_1_domain","numerical");
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_domain",outputDom);
    resource.prepare();
    QString url = resource.url().toString();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    return resource;

}



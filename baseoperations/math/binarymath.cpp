#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "binarymath.h"

using namespace Ilwis;
using namespace BaseOperations;


OperationImplementation *BinaryMath::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMath( metaid, expr);
}

BinaryMath::BinaryMath() : _coveragecoverage(false)
{
}

BinaryMath::BinaryMath(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryMath::setOutput(ExecutionContext *ctx, SymbolTable& symTable) {
    if ( ctx) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        ctx->addOutput(symTable,value,_outputGC->name(), itRASTER,_outputGC->resource() );
    }
    return _outputGC.isValid();
}

bool BinaryMath::executeRasterNumber(ExecutionContext *ctx, SymbolTable& symTable) {

    auto binaryMath = [&](const Box3D<qint32> box ) -> bool {
        PixelIterator iterIn(_inputGC1, box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(box.size()));

        double v_in = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            if ( (v_in = *iterIn) != rUNDEF) {
                switch(_operator) {
                case otPLUS:
                    v = v_in + _number;break;
                case otMINUS:
                    v = v_in - _number;break;
                case otDIV:
                    if ( _number != 0)
                        v = v_in / _number;
                    else
                        v = rUNDEF;
                    break;
                case otMULT:
                    v = v_in * _number;break;
                }
            }
            ++iterIn;
        });
        return true;
    };

    if (!OperationHelperRaster::execute(ctx, binaryMath, _outputGC))
            return false;


    return setOutput(ctx, symTable);

}

bool BinaryMath::executeRasterRaster(ExecutionContext *ctx, SymbolTable& symTable) {
    std::function<bool(const Box3D<qint32>)> binaryMath = [&](const Box3D<qint32> box ) -> bool {
    //auto binaryMath = [&](const Box3D<qint32> box ) -> bool {
        PixelIterator iterIn1(_inputGC1, box);
        PixelIterator iterIn2(_inputGC2, box);
        PixelIterator iterOut(_outputGC, Box3D<qint32>(box.size()));

        double v_in1 = 0;
        double v_in2 = 0;
        for_each(iterOut, iterOut.end(), [&](double& v){
            v_in1 = *iterIn1;
            v_in2 = *iterIn2;
            if ( v_in1 != rUNDEF && v_in2 != rUNDEF) {
                switch(_operator) {
                case otPLUS:
                    v = v_in1 + v_in2;break;
                case otMINUS:
                    v = v_in1 - v_in2;break;
                case otDIV:
                    if ( v_in2 != 0)
                        v = v_in1 / v_in2;
                    else
                        v = rUNDEF;
                    break;
                case otMULT:
                    v = v_in1 * v_in2;break;
                }
            }
            ++iterIn1;
            ++iterIn2;
        });
        return true;
    };

    bool res = OperationHelperRaster::execute(ctx, binaryMath, _outputGC);

    if (res)
        return setOutput(ctx, symTable);

    return false;
}

bool BinaryMath::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if ( _coveragecoverage) {
        return executeRasterRaster(ctx, symTable);

    } else  {
        return executeRasterNumber(ctx, symTable);
    }
    return true;
}

bool BinaryMath::prepareRasterRaster() {
    QString gc =  _expression.parm(0).value();
    if (!_inputGC1.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    gc =  _expression.parm(1).value();
    if (!_inputGC2.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    bool isNumeric = _inputGC1->datadef().domain()->ilwisType() == itNUMERICDOMAIN && _inputGC2->datadef().domain()->ilwisType() == itNUMERICDOMAIN;
    if (!isNumeric)
        return false;

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(0),
                                itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange1 = _inputGC1->datadef().range().dynamicCast<NumericRange>();
    if (nrange1.isNull())
        return false;

    auto nrange2 = _inputGC2->datadef().range().dynamicCast<NumericRange>();
    if (nrange2.isNull())
        return false;

    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = nrange1->min() + nrange2->min();
           rmax = nrange1->max() + nrange1->max();
           break;
        case otMINUS:
            rmin = nrange1->min() - nrange2->min();
            rmax = nrange1->max() - nrange1->max();
            break;
        case otDIV:
            rmin = nrange2->min() != 0 ? nrange1->min() / nrange2->min() : std::min(nrange1->min(), nrange2->min());
            rmax = nrange2->max() != 0 ? nrange1->max() / nrange2->max() : std::min(nrange1->max(), nrange2->max());
            break;
        case otMULT:
            rmin = nrange1->min() * nrange2->min();
            rmax = nrange1->max() * nrange1->max();
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              std::min(nrange1->step(), nrange2->step()));

    IDomain dom;
    dom.prepare("value");
    _outputGC->datadef().domain(dom);
    _outputGC->datadef().range(newRange);

    _coveragecoverage = true;
    return true;
}

bool BinaryMath::prepareRasterNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMBER) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    QString gc =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(gc)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(gc, ""));
        return false;
    }
    if(_inputGC1->datadef().domain()->ilwisType() != itNUMERICDOMAIN)
        return false;

    _number = _expression.parm(nindex).value().toDouble();

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, _expression.parm(mindex),
                                itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange = _inputGC1->datadef().range().dynamicCast<NumericRange>();
    if (nrange.isNull())
        return false;
    double rmax, rmin;
    switch(_operator) {
        case otPLUS:
           rmin = nrange->min() + _number;
           rmax = nrange->max() + _number;
           break;
        case otMINUS:
            rmin = nrange->min() - _number;
            rmax = nrange->max() - _number;
            break;
        case otDIV:
            rmin = _number != 0 ? nrange->min() / _number : nrange->min();
            rmax = _number != 0 ? nrange->max() / _number : nrange->max();
            break;
        case otMULT:
            rmin = nrange->min() * _number;
            rmax = nrange->max() * _number;
            break;
    }
    NumericRange *newRange = new NumericRange(rmin,
                                              rmax,
                                              nrange->step());

    IDomain dom;
    dom.prepare("value");
    _outputGC->datadef().domain(dom);
    _outputGC->datadef().range(newRange);

    return true;
}

OperationImplementation::State BinaryMath::prepare(ExecutionContext *,const SymbolTable&) {
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();

    QString oper = _expression.parm(2).value();
    if ( oper.toLower() == "add")
        _operator = otPLUS;
    else if ( oper.toLower() == "substract")
        _operator = otMINUS;
    else if ( oper == "divide")
        _operator = otDIV;
    else
        _operator = otMULT;

    if ( (ptype1 == itRASTER && hasType(ptype2,itNUMBER)) || (ptype2 == itRASTER && hasType(ptype1,itNUMBER)) ) {
        if(!prepareRasterNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareRasterRaster())
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 BinaryMath::createMetadata()
{
    QString url = QString("ilwis://operations/binarymathraster");
    Resource res(QUrl(url), itOPERATIONMETADATA);
    res.addProperty("namespace","ilwis");
    res.addProperty("longname","binarymathraster");
    res.addProperty("syntax","binarymathraster(gridcoverage1,gridcoverage2|number,add|substract|divide|times|mod)");
    res.addProperty("description",TR("generates a new numrical gridcoverage based on the operation, applied to all the pixels"));
    res.addProperty("inparameters","3");
    res.addProperty("pin_1_type", itRASTER | itNUMBER);
    res.addProperty("pin_1_name", TR("input gridcoverage or number"));
    res.addProperty("pin_1_domain","value");
    res.addProperty("pin_1_desc",TR("input gridcoverage with a numerical domain or number"));
    res.addProperty("pin_2_type", itRASTER | itNUMBER);
    res.addProperty("pin_2_name", TR("input gridcoverage or number"));
    res.addProperty("pin_2_domain","value");
    res.addProperty("pin_2_desc",TR("input gridcoverage with a numerical domain or number"));
    res.addProperty("pin_3_type", itSTRING);
    res.addProperty("pin_3_name", TR("Operator"));
    res.addProperty("pin_3_domain","string");
    res.addProperty("pin_3_desc",TR("operator (add, substract,divide, multiply) applied to the other 2 input operators"));
    res.addProperty("outparameters",1);
    res.addProperty("pout_1_type", itRASTER);
    res.addProperty("pout_1_name", TR("output gridcoverage"));
    res.addProperty("pout_1_domain","value");
    res.addProperty("pout_1_desc",TR("output gridcoverage with a numerical domain"));

    res.prepare();
    url += "=" + QString::number(res.id());
    res.setUrl(url);

    mastercatalog()->addItems({res});
    return res.id();
}

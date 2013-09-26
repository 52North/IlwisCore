#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "binarymathraster.h"

using namespace Ilwis;
using namespace BaseOperations;


OperationImplementation *BinaryMathRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMathRaster( metaid, expr);
}

BinaryMathRaster::BinaryMathRaster() : _coveragecoverage(false)
{
}

BinaryMathRaster::BinaryMathRaster(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryMathRaster::setOutput(ExecutionContext *ctx, SymbolTable& symTable) {
    if ( ctx) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        ctx->addOutput(symTable,value,_outputGC->name(), itRASTER,_outputGC->source() );
    }
    return _outputGC.isValid();
}

bool BinaryMathRaster::executeCoverageNumber(ExecutionContext *ctx, SymbolTable& symTable) {

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

bool BinaryMathRaster::executeCoverageCoverage(ExecutionContext *ctx, SymbolTable& symTable) {
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

    bool resource = OperationHelperRaster::execute(ctx, binaryMath, _outputGC);

    if (resource)
        return setOutput(ctx, symTable);

    return false;
}

bool BinaryMathRaster::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if ( _coveragecoverage) {
        return executeCoverageCoverage(ctx, symTable);

    } else  {
        return executeCoverageNumber(ctx, symTable);
    }
    return true;
}

bool BinaryMathRaster::prepareCoverageCoverage() {
    QString raster =  _expression.parm(0).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    raster =  _expression.parm(1).value();
    if (!_inputGC2.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
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
    dom.prepare("numerical");
    _outputGC->datadef().domain(dom);
    _outputGC->datadef().range(newRange);

    _coveragecoverage = true;
    return true;
}

bool BinaryMathRaster::prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMBER) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    QString raster =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
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
    dom.prepare("numerical");
    _outputGC->datadef().domain(dom);
    _outputGC->datadef().range(newRange);

    return true;
}

OperationImplementation::State BinaryMathRaster::prepare(ExecutionContext *,const SymbolTable&) {
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
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 BinaryMathRaster::createMetadata()
{
    QString url = QString("ilwis://operations/binarymathraster");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","binarymathraster");
    resource.addProperty("syntax","binarymathraster(gridcoverage1,gridcoverage2|number,add|substract|divide|times|mod)");
    resource.addProperty("description",TR("generates a new numrical rastercoverage/featurecoverage based on the operation, applied to all the pixels"));
    resource.addProperty("inparameters","3");
    resource.addProperty("pin_1_type", itRASTER | itNUMBER);
    resource.addProperty("pin_1_name", TR("input rastercoverage or number"));
    resource.addProperty("pin_1_domain","numerical");
    resource.addProperty("pin_1_desc",TR("input rastercoverage with a numerical domain or number"));
    resource.addProperty("pin_2_type", itRASTER | itNUMBER);
    resource.addProperty("pin_2_name", TR("input rastercoverage or number"));
    resource.addProperty("pin_2_domain","numerical");
    resource.addProperty("pin_2_desc",TR("input rastercoverage with a numerical domain or number"));
    resource.addProperty("pin_3_type", itSTRING);
    resource.addProperty("pin_3_name", TR("Operator"));
    resource.addProperty("pin_3_domain","string");
    resource.addProperty("pin_3_desc",TR("operator (add, substract,divide, multiply) applied to the other 2 input operators"));
    resource.addProperty("outparameters",1);
    resource.addProperty("pout_1_type", itRASTER);
    resource.addProperty("pout_1_name", TR("output rastercoverage"));
    resource.addProperty("pout_1_domain","numerical");
    resource.addProperty("pout_1_desc",TR("output rastercoverage with a numerical domain"));

    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}

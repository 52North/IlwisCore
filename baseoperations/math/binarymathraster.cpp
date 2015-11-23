#include <functional>
#include <future>
#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "numericoperation.h"
#include "binarymathraster.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(BinaryMathRaster)

OperationImplementation *BinaryMathRaster::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMathRaster( metaid, expr);
}

BinaryMathRaster::BinaryMathRaster() : _coveragecoverage(false)
{
}

BinaryMathRaster::BinaryMathRaster(quint64 metaid,const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryMathRaster::setOutput(ExecutionContext *ctx, SymbolTable& symTable) {
    if ( ctx) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        ctx->setOutput(symTable,value,_outputGC->name(), itRASTER,_outputGC->source() );
    }
    return _outputGC.isValid();
}

bool BinaryMathRaster::executeCoverageNumber(ExecutionContext *ctx, SymbolTable& symTable) {

    quint64 currentCount = 0;
    auto binaryMath = [&](const BoundingBox box ) -> bool {
        PixelIterator iterIn(_inputGC1, box);
        PixelIterator iterOut(_outputGC, BoundingBox(box.size()));

        std::for_each(iterOut, iterOut.end(), [&](double& v){

            v = _firstorder ? calc(_number1, *iterIn) : calc(*iterIn, _number1);
            ++iterIn;
            updateTranquilizer(currentCount++, 1000);
        });
        return true;
    };

    ctx->_threaded = false;
    if (!OperationHelperRaster::execute(ctx, binaryMath, _outputGC))
            return false;


    return setOutput(ctx, symTable);

}

bool BinaryMathRaster::executeCoverageCoverage(ExecutionContext *ctx, SymbolTable& symTable) {
    quint64 currentCount = 0;
    std::function<bool(const BoundingBox)> binaryMath = [&](const BoundingBox box ) -> bool {
        PixelIterator iterIn1(_inputGC1, box);
        PixelIterator iterIn2(_inputGC2, box);
        PixelIterator iterOut(_outputGC, box);
       // trq()->prepare(TR("BinaryMathRaster"),"Executes a binary operation on rasters", _inputGC1->size().linearSize());

        PixelIterator iterEnd = end(iterOut);
        while(iterOut != iterEnd) {
            *iterOut = calc(*iterIn1, *iterIn2);
            ++iterIn1;
            ++iterIn2;
            ++iterOut;
            updateTranquilizer(currentCount++, 1000);
        };
        return true;
    };

    if ( !_inputGC1->georeference()->isCompatible(_inputGC2->georeference())) {
        if (!OperationHelperRaster::resample(_inputGC1, _inputGC2, ctx)) {
            return ERROR2(ERR_COULD_NOT_CONVERT_2, TR("georeferences"), TR("common base"));
        }
    }
    // TODO:, research this exception
    // because of the swapping mechanism it is probably detrimental to use multithread here as blocks may continously be swapping
    //if ( _inputGC1 == _inputGC2)
        ctx->_threaded = false;

    if (OperationHelperRaster::execute(ctx, binaryMath, _outputGC))
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
    bool isNumeric = _inputGC1->datadef().domain<>()->ilwisType() == itNUMERICDOMAIN && _inputGC2->datadef().domain<>()->ilwisType() == itNUMERICDOMAIN;
    if (!isNumeric)
        return false;

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange1 = _inputGC1->datadef().range().dynamicCast<NumericRange>();
    if (nrange1.isNull())
        return false;

    auto nrange2 = _inputGC2->datadef().range().dynamicCast<NumericRange>();
    if (nrange2.isNull())
        return false;

    NumericRange *newRange = constructRangeFrom(nrange1, nrange2);

    IDomain dom;
    dom.prepare("value");
    _outputGC->datadefRef().domain(dom);
    _outputGC->datadefRef().range(newRange);

    for(quint32 i = 0; i < _outputGC->size().zsize(); ++i){
     QString index = _outputGC->stackDefinition().index(i);
        _outputGC->setBandDefinition(index,DataDefinition(dom,newRange->clone()));
    }

    _coveragecoverage = true;
    return true;
}

bool BinaryMathRaster::prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMBER) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    _firstorder = mindex > nindex;
    QString raster =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    if(_inputGC1->datadef().domain<>()->ilwisType() != itNUMERICDOMAIN)
        return false;

    _number1 = _expression.parm(nindex).value().toDouble();

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC,itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    auto nrange = _inputGC1->datadef().range<NumericRange>();
    if (nrange.isNull())
        return false;

    NumericRange *newRange = constructRangeFrom(nrange, _number1);

    IDomain dom;
    dom.prepare("value");
    _outputGC->datadefRef().domain(dom);
    _outputGC->datadefRef().range(newRange);

    for(quint32 i = 0; i < _outputGC->size().zsize(); ++i){
        QString index = _outputGC->stackDefinition().index(i);
        _outputGC->setBandDefinition(index,DataDefinition(dom,newRange->clone()));
    }

    return true;
}

OperationImplementation::State BinaryMathRaster::prepare(ExecutionContext *,const SymbolTable&) {
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();
    QString outputName = _expression.parm(0,false).value();

    mathoperator(_expression.parm(2).value());

    if ( (ptype1 == itRASTER && hasType(ptype2,itNUMBER)) || (ptype2 == itRASTER && hasType(ptype1,itNUMBER)) ) {
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    }
    initialize(_outputGC->size().linearSize());

    if ( outputName != sUNDEF)
        _outputGC->name(outputName);

    return sPREPARED;
}

quint64 BinaryMathRaster::createMetadata()
{

    OperationResource operation({"ilwis://operations/binarymathraster"});
    operation.setSyntax("binarymathraster(gridcoverage1,gridcoverage2|number,rasteroperation=!add|substract|divide|times|mod|power)");
    operation.setDescription(TR("generates a new numrical rastercoverage/featurecoverage based on the operation, applied to all the pixels"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER | itNUMBER, TR("raster or number"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"));
    operation.addInParameter(2,itSTRING , TR("operator"),TR("operator (add, substract,divide, multiply) applied to the other 2 input operators"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster, operation");

    mastercatalog()->addItems({operation});
    return operation.id();
}

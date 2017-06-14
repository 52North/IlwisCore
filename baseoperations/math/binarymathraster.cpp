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
        logOperation(_outputGC, _expression);
        ctx->setOutput(symTable,value,_outputGC->name(), itRASTER,_outputGC->resource() );
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

    if( _leftValue!= rUNDEF && _rightValue != rUNDEF){
        double result = calc(_leftValue, _rightValue);
        QVariant v = result;
        ctx->setOutput(symTable,v,sUNDEF, itDOUBLE, Resource() );
    }else if ( _coveragecoverage) {
        return executeCoverageCoverage(ctx, symTable);

    } else  {
        return executeCoverageNumber(ctx, symTable);
    }
    return true;
}

bool BinaryMathRaster::prepareCoverageCoverage() {
    QString raster = _expression.parm(0).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    raster = _expression.parm(1).value();
    if (!_inputGC2.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    if (_inputGC1->datadef().domain<>()->ilwisType() != itNUMERICDOMAIN) {
        kernel()->issues()->log(TR("Math operation attempted on non-numeric raster %1").arg(_inputGC1->name()));
        return false;
    }
    if (_inputGC2->datadef().domain<>()->ilwisType() != itNUMERICDOMAIN) {
        kernel()->issues()->log(TR("Math operation attempted on non-numeric raster %1").arg(_inputGC2->name()));
        return false;
    }

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
    dom.prepare("code=domain:value");
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
    if(_inputGC1->datadef().domain<>()->ilwisType() != itNUMERICDOMAIN) { // an itCOLORDOMAIN fails here, without reporting a clear reason to the user
        kernel()->issues()->log(TR("Math operation attempted on non-numeric raster %1").arg(_inputGC1->name()));
        return false;
    }

    _number1 = _expression.parm(nindex).value().toDouble();

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

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

OperationImplementation::State BinaryMathRaster::prepare(ExecutionContext *ctx,const SymbolTable &st) {
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();
    QString outputName = _expression.parm(0,false).value();

    if (!mathoperator( OperationHelper::unquote(_expression.parm(2).value()))){
        kernel()->issues()->log(TR("unknown operator " ) +  _expression.parm(2).value());
        return sPREPAREFAILED;
    }

    if ( (hasType(ptype1,itRASTER) && hasType(ptype2,itNUMBER)) || (hasType(ptype2,itRASTER) && hasType(ptype1,itNUMBER)) ) {
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;

    } else if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    }
    if ( ptype1 & ptype2 & itNUMBER){
        bool ok1, ok2;
        _leftValue =  _expression.input<double>(0, ok1) ;
        _rightValue = _expression.input<double>(1, ok2) ;
        if ( !(ok1 && ok2)){
            kernel()->issues()->log(TR("Illegal values used in binarymath expression:") + QString::number(_leftValue) + "  " +  QString::number(_rightValue));
            return sPREPAREFAILED;
        }

    }else{
        initialize(_outputGC->size().linearSize());

        if ( outputName != sUNDEF)
            _outputGC->name(outputName);
    }

    return sPREPARED;
}

quint64 BinaryMathRaster::createMetadata()
{

    OperationResource operation({"ilwis://operations/binarymathraster"});
    operation.setSyntax("binarymathraster(gridcoverage|number,gridcoverage|number,rasteroperation=!add|subtract|divide|times|mod|power)");
    operation.setDescription(TR("Generates a new numerical rastercoverage/featurecoverage based on the operation, applied to all the pixels"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("raster or number"), TR("Rasters with numerical domain"));
    operation.addInParameter(2,itSTRING , TR("operator"),TR("operator (add, subtract,divide, multiply) applied to the other 2 input operators"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"domain=numericdomain");
    operation.addValidation(1,1,"domain=numericdomain");
    operation.addOutParameter(0,itRASTER, TR("raster coverage"));
    operation.setKeywords("raster,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

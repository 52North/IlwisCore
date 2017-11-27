#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "binarylogical.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(BinaryLogical)

OperationImplementation *BinaryLogical::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryLogical( metaid, expr);
}

BinaryLogical::BinaryLogical() : _coveragecoverage(false)
{
}

BinaryLogical::BinaryLogical(quint64 metaid,const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr) , _coveragecoverage(false)
{
}

bool BinaryLogical::setOutput(ExecutionContext *ctx, SymbolTable& symTable) {
    if ( ctx) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outputGC);
        _outputGC->addDescription(_expression.toString());
        ctx->setOutput(symTable,value,_outputGC->name(), itRASTER, _outputGC->resource() );
    }
    return _outputGC.isValid();
}

bool BinaryLogical::executeCoverageNumber(ExecutionContext *ctx, SymbolTable& symTable) {

    auto BinaryLogical = [&](const BoundingBox box ) -> bool {
        PixelIterator iterIn(_inputGC1, box);
        PixelIterator iterOut(_outputGC, BoundingBox(box.size()));

        std::for_each(iterOut, iterOut.end(), [&](double& v){
            double v_in1 = *iterIn;
            v = compare2(_operator, v_in1, _number, true);
            ++iterIn;
        });
        return true;
    };

    ctx->_threaded = false;
    if (!OperationHelperRaster::execute(ctx, BinaryLogical, _outputGC))
        return false;

    return setOutput(ctx, symTable);
}

bool BinaryLogical::executeCoverageCoverage(ExecutionContext *ctx, SymbolTable& symTable) {
    std::function<bool(const BoundingBox&)> binaryLogical = [&](const BoundingBox& box ) -> bool {
        PixelIterator iterIn1(_inputGC1, box);
        PixelIterator iterIn2(_inputGC2, box);
        PixelIterator iterOut(_outputGC, BoundingBox(box.size()));

        double v_in1 = 0;
        double v_in2 = 0;
        std::for_each(iterOut, iterOut.end(), [&](double& v){
            v_in1 = *iterIn1;
            v_in2 = *iterIn2;
            v = compare2(_operator, v_in1, v_in2, false);
            ++iterIn1;
            ++iterIn2;
        });
        return true;
    };

    if ( !_inputGC1->georeference()->isCompatible(_inputGC2->georeference())) {
        if (!OperationHelperRaster::resample(_inputGC1, _inputGC2, ctx)) {
            return ERROR2(ERR_COULD_NOT_CONVERT_2, TR("georeferences"), TR("common base"));
        }
    }

    ctx->_threaded = false;
    bool resource = OperationHelperRaster::execute(ctx, binaryLogical, _outputGC);

    if (resource && ctx)
        return setOutput(ctx, symTable);

    return false;
}

bool BinaryLogical::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if ( _coveragecoverage) {
        return executeCoverageCoverage(ctx, symTable);

    } else  {
        return executeCoverageNumber(ctx,symTable);
    }
    return true;
}

bool BinaryLogical::prepareCoverageCoverage() {
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
    if (!isNumeric ){
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"domain","logical operation");
    }

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    IDomain dom;
    dom.prepare("boolean");
    _outputGC->datadefRef().domain(dom);

    for(quint32 i = 0; i < _outputGC->size().zsize(); ++i){
        QString index = _outputGC->stackDefinition().index(i);
        _outputGC->setBandDefinition(index,DataDefinition(dom));
    }
    _coveragecoverage = true;
    return true;
}

bool BinaryLogical::prepareCoverageNumber(IlwisTypes ptype1, IlwisTypes ptype2) {

    int mindex = (ptype1 & itNUMBER) == 0 ? 0 : 1;
    int nindex = mindex ? 0 : 1;

    QString raster =  _expression.parm(mindex).value();
    if (!_inputGC1.prepare(raster)) {
        kernel()->issues()->log(TR(ERR_COULD_NOT_LOAD_2).arg(raster, ""));
        return false;
    }
    if(_inputGC1->datadef().domain<>()->ilwisType() != itNUMERICDOMAIN)
        return false;

    QString v = _expression.parm(nindex).value();
    if ( v == "?" || v == "undefined")
        _number = rUNDEF;
    else
        _number = _expression.parm(nindex).value().toDouble();

    OperationHelperRaster helper;
    _box = helper.initialize(_inputGC1, _outputGC, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    IDomain dom;
    dom.prepare("boolean");
    _outputGC->datadefRef().domain(dom);

    for(quint32 i = 0; i < _outputGC->size().zsize(); ++i){
        QString index = _outputGC->stackDefinition().index(i);
        _outputGC->setBandDefinition(index,DataDefinition(dom));
    }

    return true;
}

OperationImplementation::State BinaryLogical::prepare(ExecutionContext *ctx,const SymbolTable &st) {
    OperationImplementation::prepare(ctx,st);
    if ( _expression.parameterCount() != 3){
        return sPREPAREFAILED;
    }

    IlwisTypes ptype1 = _expression.parm(0).valuetype();
    IlwisTypes ptype2 = _expression.parm(1).valuetype();

    QString oper = _expression.parm(2).value();
    oper = oper.toLower();
    if ( oper == "and")
        _operator = loAND;
    else if ( oper == "or")
        _operator = loOR;
    else if ( oper == "xor")
        _operator = loXOR;
    else if ( oper == "less")
        _operator = loLESS;
    else if ( oper == "lesseq")
        _operator = loLESSEQ;
    else if ( oper == "neq")
        _operator = loNEQ;
    else if ( oper == "eq")
        _operator = loEQ;
    else if ( oper == "greater")
        _operator = loGREATER;
    else if ( oper == "greatereq")
        _operator = loGREATEREQ;

    if ( ptype1 & ptype2 & itRASTER ) {
        if(!prepareCoverageCoverage())
            return sPREPAREFAILED;
    } else if ( ((ptype1 | ptype2) & (itRASTER | itNUMBER)) ) {
        if(!prepareCoverageNumber(ptype1, ptype2))
            return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 BinaryLogical::createMetadata()
{
    OperationResource operation({"ilwis://operations/binarylogicalraster"});
    operation.setSyntax("binarylogicalraster(gridcoverage1,gridcoverage2|number|boolean,logicaloperation=and|or|xor|less|lesseq|neq|eq|greater|greatereq)");
    operation.setDescription(TR("generates a new boolean map based on the logical condition used"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER | itNUMBER, TR("input rastercoverage or number/boolean"),TR("input rastercoverage with a numerical/boolean domain or number"));
    operation.addInParameter(1,itRASTER | itNUMBER, TR("input rastercoverage or number/boolean"),TR("input rastercoverage with a numerical/boolean domain or number"));
    operation.addInParameter(2,itSTRING , TR("Operator"),TR("operator applied to the other 2 input parameters"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,0,"valuetype=bool");
    operation.addValidation(1,1,"valuetype=bool");
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"),TR("output rastercoverage with a boolean domain"));
    operation.setKeywords("raster,math,boolean");

    mastercatalog()->addItems({operation});
    return operation.id();
}

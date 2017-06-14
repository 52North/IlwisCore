#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "unarymath.h"
#include "unarymathtable.h"

using namespace Ilwis;
using namespace BaseOperations;

UnaryMathTable::UnaryMathTable() {

}

UnaryMathTable::UnaryMathTable(quint64 metaid, const Ilwis::OperationExpression& expr, const QString& outdom,  UnaryFunction fun) :
    UnaryMath(metaid, expr, outdom,fun)
{

}

bool UnaryMathTable::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::vector<QVariant> data = _inputTable->column(_inColumn);
    std::vector<QVariant> dataOut(data.size());
    auto iterOut = dataOut.begin();
    for(const QVariant& val : data) {
        *iterOut = _unaryFun(val.toDouble());
        ++iterOut;
    }
    _outputTable->column(_outColumn,dataOut);
    if ( _outputTable.isValid()) {
        QVariant var;
        var.setValue<ITable>(_outputTable);
        ctx->setOutput(symTable,var, _outputTable->name(),itCOLUMN,_outputTable->resource(), _outColumn);
        return true;
    }


    return true;
}

OperationImplementation::State UnaryMathTable::prepare(ExecutionContext *ctx,const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString table = _expression.parm(0).value();
    if (!_inputTable.prepare(table)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,table,"");
        return sPREPAREFAILED;
    }
    _inColumn = _inputTable->columndefinition(0).name();
    if ( _expression.parameterCount() == 2){
        _inColumn = _expression.parm(1).value();
    }
    _outColumn = "column_" + QString::number(_inputTable->columnCount());
    if (!hasType(_inputTable->columndefinition(_inColumn).datadef().domain<>()->valueType(), itNUMBER)) {
        ERROR2(ERR_NOT_COMPATIBLE2, TR("column"), _expression.name());
        return sPREPAREFAILED;
    }
    QString outName = _expression.parm(0, false).value();
    if ( outName != sUNDEF) {
        if(!_outputTable.prepare(outName)) {// output table doesnt need to exists
            _outputTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outName), _inputTable->ilwisType());
            _outputTable->name(outName);
        }
    } else
        _outputTable = _inputTable;
    IDomain dom;
    if(!dom.prepare(_outputDomain))
        return sPREPAREFAILED;
    ColumnDefinition def(_outColumn, dom);
    _outputTable->addColumn(def);
    return sPREPARED;

}

Resource UnaryMathTable::populateMetadata(const QString& item, const QString& longname) {
    OperationResource operation(item);
    operation.setSyntax(QString("%1(table,columnname)").arg(item));
    operation.setDescription(TR("generates a new numerical column based on the operation, applied to all the input column values"));
    operation.setLongName(longname);
    operation.setInParameterCount({2});
    operation.addInParameter(0,itTABLE,"input table");
    operation.addInParameter(1, itSTRING,TR("input column name"),TR("column numerical values"), OperationResource::ueCOMBO);
    operation.addValidation(0,1,"columns with domain=numericdomain");
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOLUMN, "output column name");
    operation.setKeywords("table,column,math,numeric");

    return operation;

}




#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "commandhandler.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "tablevalue.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(TableValue)

TableValue::TableValue()
{
}

TableValue::TableValue(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool TableValue::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QVariant v = _inputTable->cell(_columnName, _recordNr);
    IDomain coldom = _inputTable->columndefinition(_columnName).datadef().domain();
    if ( hasType(coldom->ilwisType(), itITEMDOMAIN|itTEXTDOMAIN)){
        QString value = coldom->impliedValue(v).toString();
        ctx->setOutput(symTable, QVariant(value), sUNDEF, itSTRING, Resource());
    }else{
        ctx->setOutput(symTable, QVariant(v.toDouble()), sUNDEF, itDOUBLE, Resource());
    }
    logOperation(_expression);
    return true;
}

Ilwis::OperationImplementation *TableValue::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new TableValue(metaid, expr);
}

Ilwis::OperationImplementation::State TableValue::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);

    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    OperationHelper::check([&] ()->bool {  _columnName =_expression.input<QString>(1); return _inputTable->columnIndex(_columnName) != iUNDEF; },
    {ERR_NOT_FOUND2,_columnName, _expression.input<QString>(0) } );

    _recordNr = _expression.input<int>(2);
    if ( _recordNr >= _inputTable->recordCount()){
        kernel()->issues()->log(TR("Record number is outside table"));
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 TableValue::createMetadata()
{

    OperationResource operation({"ilwis://operations/tablevalue"});
    operation.setSyntax("tablevalue(inputtable,columnname,recordnr)");
    operation.setLongName("Table value by record");
    operation.setDescription(TR("returns the value at location column and record number"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itTABLE , TR("input table"),TR("input table from which a cell value will be selected"));
    operation.addInParameter(1,itSTRING, TR("column"), TR("column name from which a cell value will be selected"), OperationResource::ueCOMBO);
    operation.addInParameter(2,itINTEGER, TR("record"), TR("record number in the table"));
    operation.setOutParameterCount({1});
    operation.addValidation(0,1,"columns");
    operation.addOutParameter(0,itDOUBLE , TR("value"),TR("value in the indicated cell"));
    operation.setKeywords("raster,pixel,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();

}


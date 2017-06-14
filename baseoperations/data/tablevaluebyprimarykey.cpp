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
#include "tablevaluebyprimarykey.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(TableValueByPrimaryKey)

TableValueByPrimaryKey::TableValueByPrimaryKey()
{
}

TableValueByPrimaryKey::TableValueByPrimaryKey(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool TableValueByPrimaryKey::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    IDomain primdom = _inputTable->columndefinition(_primaryColumnName).datadef().domain();
    std::vector<QVariant> primaryValues = _inputTable->column(_primaryColumnName);
    int record = 0;
    for(; record < primaryValues.size(); ++record){
        QString value = primdom->impliedValue(primaryValues[record]).toString();
        if ( value == _primaryKeyValue){
            break;
        }
    }
    QVariant v = _inputTable->cell(_valueColumn, record);
    IDomain coldom = _inputTable->columndefinition(_valueColumn).datadef().domain();
    if ( hasType(coldom->ilwisType(), itITEMDOMAIN|itTEXTDOMAIN)){
        QString value = coldom->impliedValue(v).toString();
        ctx->setOutput(symTable, QVariant(value), sUNDEF, itSTRING, Resource());
    }else{
        ctx->setOutput(symTable, QVariant(v.toDouble()), sUNDEF, itDOUBLE, Resource());
    }
    logOperation(_expression);
    return true;
}

Ilwis::OperationImplementation *TableValueByPrimaryKey::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new TableValueByPrimaryKey(metaid, expr);
}

Ilwis::OperationImplementation::State TableValueByPrimaryKey::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    OperationHelper::check([&] ()->bool {  _primaryColumnName =_expression.input<QString>(1); return _inputTable->columnIndex(_primaryColumnName) != iUNDEF; },
    {ERR_NOT_FOUND2,_primaryColumnName, _expression.input<QString>(0) } );

    _primaryKeyValue = _expression.input<QString>(2);

    OperationHelper::check([&] ()->bool {  _valueColumn =_expression.input<QString>(3); return _inputTable->columnIndex(_valueColumn) != iUNDEF; },
    {ERR_NOT_FOUND2,_valueColumn, _expression.input<QString>(0) } );


    return sPREPARED;
}

quint64 TableValueByPrimaryKey::createMetadata()
{

    OperationResource operation({"ilwis://operations/tablevalue"});
    operation.setSyntax("tablevalue(inputtable,primarycolumnname,primarykeyvalue, valuecolumn)");
    operation.setLongName("Table value by primary key");
    operation.setDescription(TR("returns the value at location column and the record number belonging to the record of the primary key"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itTABLE , TR("input table"),TR("input table from which a cell value will be selected"));
    operation.addInParameter(1,itSTRING, TR("primary key column"), TR("column name from which a cell value will be selected"), OperationResource::ueCOMBO);
    operation.addInParameter(2,itSTRING, TR("primary key value"), TR("Primary key value to be located in the primary key column. This record "));
    operation.parameterNeedsQuotes(2);
    operation.addInParameter(3,itSTRING, TR("value column"), TR("column were the value will be located"), OperationResource::ueCOMBO);
    operation.setOutParameterCount({1});
    operation.addValidation(0,1,"columns");
    operation.addValidation(0,3,"columns");
    operation.addOutParameter(0,itANY , TR("value"),TR("value in the indicated cell"));
    operation.setKeywords("raster,pixel");

    mastercatalog()->addItems({operation});
    return operation.id();

}



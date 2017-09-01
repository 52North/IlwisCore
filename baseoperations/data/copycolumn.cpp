#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "table.h"
#include "selectabletable.h"
#include "basetable.h"
#include "flattable.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "operationhelper.h"
#include "copycolumn.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CopyColumn)

CopyColumn::CopyColumn()
{

}

CopyColumn::CopyColumn(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CopyColumn::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    int colIndex = _outputTable->columnIndex(_outputColumnName);
    std::map<QVariant, QVariant> inputOrder;
    if ( _inputKey != "" && _outputKey != ""){
        auto keyvaluesIn = _inputTable->column(_inputKey);
        auto colvaluesIn  = _inputTable->column(_inputColumnName);
        for (int i=0; i < keyvaluesIn.size(); ++i)
            inputOrder[keyvaluesIn[i]] = colvaluesIn[i];
        auto keyvaluesOut = _inputTable->column(_outputKey);
        std::map<QVariant, int> outputOrder;
        for(int i=0; i < keyvaluesOut.size(); ++i){
            outputOrder[keyvaluesOut[i]]  = i;
        }
        for(auto item : inputOrder){
            auto iter = outputOrder.find(item.first);
            if ( iter != outputOrder.end()){
                _outputTable->setCell(colIndex, (*iter).second,item.second)  ;
            }
        }
    }else {
          auto colvaluesIn  = _inputTable->column(_inputColumnName);

          for(int i=0; i < colvaluesIn.size(); ++i){
            _outputTable->setCell(colIndex, i,colvaluesIn[i]);
          }
    }

    setOutput(_outputTable,ctx, symTable);

    return true;

}

Ilwis::OperationImplementation *CopyColumn::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CopyColumn(metaid, expr);
}

bool checkUnique(const ITable& table, const QString& primKey){
    std::set<QVariant> values;
    auto cvalues = table->column(primKey);
    for(const QVariant& v : cvalues){
        values.insert(v);
    }
    if ( values.size() != cvalues.size()){
        kernel()->issues()->log(TR("Invalid primary key column. Not all values are unique : ") + primKey);
        return false;
    }
    return true;
}

Ilwis::OperationImplementation::State CopyColumn::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    _inputColumnName = _expression.input<QString>(1);
    if ( _inputTable->columnIndex(_inputColumnName) == iUNDEF){
        kernel()->issues()->log(TR("Not an existing column, column can not be used for a copy : ") + _inputColumnName);
        return sPREPAREFAILED;
    }
    OperationHelper::check([&] ()->bool { return _outputTable.prepare(_expression.input<QString>(2), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(2), "" } );

    _outputColumnName = _expression.input<QString>(3);
    if ( _outputColumnName == ""){
        kernel()->issues()->log(TR("No empty names are allowed : ") );
        return sPREPAREFAILED;
    }
    if ( _outputTable->columnIndex(_outputColumnName) != iUNDEF){
        if (!_inputTable->columndefinition(_inputColumnName).datadef().domain()->isCompatibleWith(_outputTable->columndefinition(_outputColumnName).datadef().domain().ptr() )){
            kernel()->issues()->log(TR("Column domains are not compatible. No copy possible: ") + _inputColumnName + " " + _outputColumnName);
            return sPREPAREFAILED;
        }
    }else {
            _outputTable->columndefinition(_inputTable->columndefinition(_inputColumnName))    ;
    }

    _inputKey = _expression.input<QString>(4);
    if ( _inputTable->columnIndex(_inputKey) != iUNDEF){
        kernel()->issues()->log(TR("Column doesnt exist : ") + _inputKey);
        return sPREPAREFAILED;
    }
    if ( _inputKey != ""){
        if (!checkUnique(_inputTable, _inputKey)){
            return sPREPAREFAILED;
        }
    }

    _outputKey = _expression.input<QString>(5);
    if ( _outputTable->columnIndex(_outputKey) != iUNDEF){
        kernel()->issues()->log(TR("Column doesnt exist : ") + _outputKey);
        return sPREPAREFAILED;
    }
    if ( _outputKey != ""){
        if (!checkUnique(_outputTable, _outputKey)){
            return sPREPAREFAILED;
        }
    }

    return sPREPARED;
}

quint64 CopyColumn::createMetadata()
{
    OperationResource resource({"ilwis://operations/copycolumn"});
    resource.setLongName("Copy Column");
    resource.setSyntax("copycolumn(sourcetable,sourcecolumn, targettable, targetcolumn, sourceKey, targetKey))");
    resource.setDescription(TR("Copies a column for one table to another table"));
    resource.setInParameterCount({6});
    resource.addInParameter(0, itTABLE,TR("source table"), TR("Table to which from which the column will be copied"));
    resource.addInParameter(1, itSTRING,TR("source column"), TR("Name of the column to be copied"), OperationResource::ueCOMBO);
    resource.addInParameter(2, itTABLE, TR("target table"), TR("Table to which the column will be copied"));
    resource.addInParameter(3, itSTRING,TR("target column"), TR("Name of the new column"));
    resource.addInParameter(4, itSTRING,TR("primary source key"), TR("Optional key that determines the order of the values to be copied"), OperationResource::ueCOMBO);
    resource.addInParameter(5, itSTRING,TR("primary target key"), TR("Optional key that determines how values are inserted in the target"), OperationResource::ueCOMBO);
    resource.setOutParameterCount({1});
    resource.setOutputIsInput(2,0);
    resource.addValidation(0,1,"columns");
    resource.addValidation(0,4,"columns");
    resource.addValidation(2,5,"columns");
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itTABLE, TR("table"), TR("The input table with an extra column"));
    resource.setKeywords("table, column,internal");

    mastercatalog()->addItems({resource});
    return resource.id();
}


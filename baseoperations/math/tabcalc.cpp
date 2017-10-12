#include <functional>
#include <future>
#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "numericoperation.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "calculatoroperation.h"
#include "tabcalc.h"

using namespace Ilwis;
using namespace BaseOperations;

TabCalc::TabCalc()
{

}

TabCalc::TabCalc(quint64 metaid, const Ilwis::OperationExpression &expr) : CalculatorOperation(metaid, expr)
{

}

void TabCalc::preFetchColumnValues(){
    for(int i=0; i < _actions.size(); ++i){
        Action& action = _actions[i];
        for(ParmValue& parm: action._values){
            if ( parm._columName != "") {
                parm._columnValues = _inputTable->column(parm._columName);
            }
        }
    }
}

bool TabCalc::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
       if((_prepState = prepare(ctx, symTable)) != sPREPARED)
           return false;

   std::vector<QVariant> values(_inputTable->recordCount());
   preFetchColumnValues();
   for(QVariant& v : values){

       v = calc();
       updateTranquilizer(_record, 5);
       ++_record;
   }
   _outputTable->column(_outputColumn,values);
   if ( !_createNewTable){
       ctx->_additionalInfo["outputisinput"] = _inputTable->id();
   }

   QVariant value;
   value.setValue<ITable>(_outputTable);
   logOperation(_outputTable, _expression);
   ctx->setOutput(symTable,value,_outputTable->name(), itTABLE,_outputTable->resource() );

   return true;
}

OperationImplementation::State TabCalc::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString expr = _expression.input<QString>(0);

    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(1), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );

    _outputColumn = _expression.input<QString>(2);

     _createNewTable = _expression.input<bool>(_expression.parameterCount() - 1);

    for(int parmIndex = 3 ; parmIndex < _expression.parameterCount()-1; ++parmIndex){
        Parameter parm = _expression.parm(parmIndex);
        if ( hasType(parm.valuetype(), itSTRING)){
            QString columnName = parm.value();
            int index = _inputTable->columnIndex(columnName);
            if(index == iUNDEF){
                kernel()->issues()->log(TR("Column ") + columnName + " " + TR(" does not exist in ") + _inputTable->name());
                return sPREPAREFAILED;
            }
            _inputColumns[parmIndex - 2] =columnName; // index starts at three but the variables are a 1 based index system ( e.g @1+@2)
        }else if ( hasType(parm.valuetype(), itNUMBER)){
            bool ok;
            double v = parm.value().toDouble(&ok);
            if (!ok){
                return sPREPAREFAILED;
            }
            _inputNumbers[parmIndex-2] = v;
        }
    }
    _outputTable = _createNewTable ? _inputTable->copyTable(sUNDEF) : _inputTable ;

    IDomain outputDomain;
    try{
        outputDomain = linearize(shuntingYard(expr));
        if (!outputDomain.isValid()){
            return sPREPAREFAILED;
        }
    } catch(ErrorObject& err){
        return sPREPAREFAILED;
    }


    _outputTable->addColumn(ColumnDefinition(_outputColumn,DataDefinition(outputDomain),_outputTable->columnCount()));
    initialize(_inputTable->recordCount());

    return sPREPARED;
}

bool TabCalc::check(int index) const {
    auto iter = _inputColumns.find(index);
    if ( iter == _inputColumns.end() ){
        auto iter2 = _inputNumbers.find(index);
        if ( iter2 == _inputNumbers.end()){
            return false;
        }
    }
    return true;
}

void TabCalc::fillValues(int pindex,const QString& part, ParmValue& val)  {
    auto iterP = _inputColumns.find(pindex);
    if ( iterP != _inputColumns.end()){
        val._type = CalculatorOperation::COLUMN;
        val._columName = (*iterP).second;

    }else {
        auto iterP = _inputNumbers.find(pindex);
        val._type = CalculatorOperation::NUMERIC;
        val._value = (*iterP).second;
    }
}

DataDefinition TabCalc::datadef(int index)
{
    auto iterP = _inputColumns.find(index);
    if ( iterP != _inputColumns.end()){
        ColumnDefinition coldef = _inputTable->columndefinition(_inputColumns[index]);
        return coldef.datadef();
    }
    return DataDefinition();
}

//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc1)
quint64 TabCalc1::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 1");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column or a number"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number"), TR("first column to be used in teh expression"));
     operation.addInParameter(4,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc1::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc1( metaid, expr);
}

TabCalc1::TabCalc1(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}

//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc2)
quint64 TabCalc2::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 2");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number 1"), TR("Name of the first column to be used in teh expression or a number"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number 2"), TR("Name of the second column to be used in teh expression or a number"));
     operation.addInParameter(5,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc2::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc2( metaid, expr);
}

TabCalc2::TabCalc2(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}

//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc3)
quint64 TabCalc3::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 3");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn, inputcolumn, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({7});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number 1"), TR("Name of the first column to be used in teh expression or a number"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number 2"), TR("Name of the second column to be used in teh expression or a number"));
     operation.addInParameter(5,itSTRING|itNUMBER, TR("input column/number 3"), TR("Name of the third column to be used in teh expression or a number"));
     operation.addInParameter(6,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc3::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc3( metaid, expr);
}

TabCalc3::TabCalc3(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}

//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc4)
quint64 TabCalc4::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 4");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn, inputcolumn, inputcolumn, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({8});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number 1"), TR("Name of the first column to be used in teh expression or a number"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number 2"), TR("Name of the second column to be used in teh expressionor a number"));
    operation.addInParameter(5,itSTRING|itNUMBER, TR("input column/number 3"), TR("Name of the third column to be used in teh expression or a number"));
    operation.addInParameter(6,itSTRING|itNUMBER, TR("input column/number 4"), TR("Name of the fourth column to be used in teh expressionor a number"));
     operation.addInParameter(7,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc4::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc4( metaid, expr);
}

TabCalc4::TabCalc4(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}
//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc5)
quint64 TabCalc5::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 5");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn, inputcolumn, inputcolumn, inputcolumn, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({9});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number 1"), TR("Name of the first column to be used in teh expression or a number"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number 2"), TR("Name of the second column to be used in teh expressionor a number"));
    operation.addInParameter(5,itSTRING|itNUMBER, TR("input column/number 3"), TR("Name of the third column to be used in teh expression or a number"));
    operation.addInParameter(6,itSTRING|itNUMBER, TR("input column/number 4"), TR("Name of the fourth column to be used in teh expressionor a number"));
    operation.addInParameter(7,itSTRING|itNUMBER, TR("input column/number 5"), TR("Name of the fifth column to be used in teh expressionor a number"));
     operation.addInParameter(8,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc5::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc5( metaid, expr);
}

TabCalc5::TabCalc5(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}
//--------------------------------------------------------------------------
REGISTER_OPERATION(TabCalc6)
quint64 TabCalc6::createMetadata()
{
    OperationResource operation({"ilwis://operations/tabcalc"});
    operation.setLongName("TabCalc 6");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn, inputcolumn, inputcolumn, inputcolumn, inputcolumn, inputcolumn)");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({10});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
    operation.addInParameter(3,itSTRING|itNUMBER, TR("input column/number 1"), TR("Name of the first column to be used in teh expression or a number"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number 2"), TR("Name of the second column to be used in teh expressionor a number"));
    operation.addInParameter(5,itSTRING|itNUMBER, TR("input column/number 3"), TR("Name of the third column to be used in teh expression or a number"));
    operation.addInParameter(6,itSTRING|itNUMBER, TR("input column/number 4"), TR("Name of the fourth column to be used in teh expressionor a number"));
    operation.addInParameter(7,itSTRING|itNUMBER, TR("input column/number 5"), TR("Name of the fifth column to be used in teh expressionor a number"));
    operation.addInParameter(8,itSTRING|itNUMBER, TR("input column/number 6"), TR("Name of the sixth column to be used in teh expressionor a number"));
     operation.addInParameter(9,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc6::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc6( metaid, expr);
}

TabCalc6::TabCalc6(quint64 metaid,const Ilwis::OperationExpression &expr) : TabCalc(metaid, expr)
{}

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

OperationImplementation::State TabCalc::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString expr = _expression.input<QString>(0);

    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(1), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(1), "" } );

    _outputColumn = _expression.input<QString>(2);

     _copyTable = _expression.input<bool>(3);

    for(int parmIndex = 4 ; parmIndex < _expression.parameterCount(); ++parmIndex){
        Parameter parm = _expression.parm(parmIndex);
        if ( hasType(parm.valuetype(), itSTRING)){
            QString columnName = parm.value();
            int index = _inputTable->columnIndex(columnName);
            if(index == iUNDEF){
                return sPREPAREFAILED;
            }
            _inputColumns[parmIndex] =columnName;
        }else if ( hasType(parm.valuetype(), itNUMBER)){
            bool ok;
            double v = parm.value().toDouble(&ok);
            if (!ok){
                return sPREPAREFAILED;
            }
            _inputNumbers[parmIndex] = v;
        }
    }
    OperationHelperRaster helper;
    helper.initialize((*_inputRasters.begin()).second.raster(), _outputRaster, itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF);

    IDomain outputDomain = linearize(shuntingYard(expr));
    if( !outputDomain.isValid())
        return sPREPAREFAILED;

    _outputRaster->datadefRef().domain(outputDomain);

    for(quint32 i = 0; i < _outputRaster->size().zsize(); ++i){
        QString index = _outputRaster->stackDefinition().index(i);
        _outputRaster->setBandDefinition(index,DataDefinition(outputDomain));
    }

    return sPREPARED;
}

//--------------------------------------------------------------------------
REGISTER_OPERATION(MapCalc1)
quint64 TabCalc1::createMetadata()
{
    OperationResource operation({"ilwis://operations/mapcalc"});
    operation.setLongName("TabCalc 1");
    operation.setSyntax("tabcalc(expression,inputtable,outputcolumn,true|!false, inputcolumn");
    operation.setDescription(TR("Generates a table with a new column based on the expression. The column is either in a new table or in the original table"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itSTRING, TR("Expression"), TR("The expression is an abstract expression were the numbers indicate indexes in the parameter list"));
    operation.parameterNeedsQuotes(0);
    operation.addInParameter(1,itTABLE, TR("input table"), TR("The table that contains the input column(s)"));
    operation.addInParameter(2,itSTRING, TR("output column"), TR("Name of the new column"));
     operation.addInParameter(3,itBOOL, TR("Create new table"), TR("The column is either in a new table or in the original table"));
    operation.addInParameter(4,itSTRING|itNUMBER, TR("input column/number"), TR("Name of the new column"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output table"));
    operation.setKeywords("table,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}

OperationImplementation *TabCalc1::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new TabCalc1( metaid, expr);
}

TabCalc1::TabCalc1(quint64 metaid,const Ilwis::OperationExpression &expr) : MapCalc(metaid, expr)
{}

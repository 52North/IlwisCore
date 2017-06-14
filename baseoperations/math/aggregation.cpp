#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "domain.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "numericoperation.h"
#include "tranquilizer.h"
#include "aggregation.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Aggregation)


Aggregation::Aggregation()
{
}

Aggregation::Aggregation(quint64 metaid, const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
{

}

OperationImplementation *Aggregation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Aggregation(metaid, expr);
}

bool Aggregation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::vector<QVariant> data1(_inputTable->recordCount(), _number1), data2(_inputTable->recordCount(), _number2);
    if ( _column1 != sUNDEF)
        data1 = _inputTable->column(_column1);

    if ( _column2 != sUNDEF)
        data2 = _inputTable->column(_column2);

    auto iter2 = data2.begin();

    std::map<QVariant,double> aggregates;
    for(auto value : data1){
       aggregates[(*iter2)] += value.toDouble();
       ++iter2;
    }
    std::vector<QVariant> out1(aggregates.size());
    std::vector<QVariant> out2(aggregates.size());

    int index = 0;
    for(std::map<QVariant,double>::iterator iter = aggregates.begin(); iter != aggregates.end(); ++iter) {
        out1[index] = (*iter).first;
        out2[index] = (*iter).second;
        ++index;
    }

    _outputTable->column(_column2, out1);
    _outputTable->column(_column1, out2);

     if ( _outputTable.isValid()) {
            QVariant var;
            var.setValue<ITable>(_outputTable);
            logOperation(_outputTable, _expression);
            ctx->setOutput(symTable,var, _outputTable->name(),itTABLE,_outputTable->resource(),_outColumn1);
            return true;
    }

    return false;
}

Ilwis::OperationImplementation::State Aggregation::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString table = _expression.parm(0).value();
    if (!_inputTable.prepare(table)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,table,"");
        return sPREPAREFAILED;
    }
    const Parameter& parm1 = _expression.parm(1);
    const Parameter& parm2 = _expression.parm(2);
    ColumnDefinition def1, def2;
    bool ok = true;
    if ( hasType(parm1.valuetype(),itNUMBER)){
        _number1 = parm1.value().toDouble(&ok);
    }else{
         _column1 = parm1.value();
         def1 = _inputTable->columndefinition(_column1);
    }

    if ( hasType(parm2.valuetype(),itNUMBER)){
        _number2 = parm2.value().toDouble(&ok);
    } else{
        _column2 = parm2.value();
        def2 = _inputTable->columndefinition(_column2);
    }
    if (!(hasType(def1.datadef().domain<>()->valueType(),itNUMBER))){
        ERROR2(ERR_NOT_COMPATIBLE2, TR("column"), _expression.name());
        return sPREPAREFAILED;
    }
    _outColumn1 = _expression.parm(1).value();
    _outColumn2 = _expression.parm(2).value();

    QString outName = _expression.parm(0, false).value();
    if(table != outName){
        if (outName == sUNDEF) {
            IFlatTable  ftable;
            ftable.prepare();
            _outputTable = ftable;
        }else if(!_outputTable.prepare(outName)) {// output table doesnt need to exists
            _outputTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outName), _inputTable->ilwisType());
            _outputTable->name(outName);

        }
    }
    else{
        _outputTable = _inputTable;
    }
    IDomain dom;
    dom.prepare("value");
    ColumnDefinition coldef(_outColumn1,dom);
    NumericRange *rng = new NumericRange(0,100000000000000000);
    coldef.datadef().range(rng);
    _outputTable->addColumn(coldef);
    _outputTable->addColumn(def2);

    return sPREPARED;
}


quint64 Aggregation::createMetadata()
{
    OperationResource operation({"ilwis://operations/aggregation"});
    operation.setLongName("Aggregation");
    operation.setSyntax("aggregation(input-table, column-name|number,column-name|number)");
    operation.setDescription(TR("Aggregate the values of a column based on second input column"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itTABLE, TR("input table-column"),TR("input table-column from which the input column will be chosen"));
    operation.addInParameter(1,itSTRING | itNUMBER, TR("input column name or number"),TR("column with a numerical domain or number"), OperationResource::ueCOMBO);
    operation.addInParameter(2,itSTRING | itNUMBER , TR("input column name or number"),TR("column with a numerical domain or number"), OperationResource::ueCOMBO);
    operation.setOutParameterCount({1});
    operation.addValidation(0,1,"columns with domain=numericdomain");
    operation.addValidation(0,2,"columns with domain=numericdomain");
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,column,aggregate,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}


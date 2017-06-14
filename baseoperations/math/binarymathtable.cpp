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
#include "binarymathtable.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(BinaryMathTable)

BinaryMathTable::BinaryMathTable()
{
}

BinaryMathTable::BinaryMathTable(quint64 metaid, const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr)
{

}


OperationImplementation *BinaryMathTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new BinaryMathTable(metaid, expr);
}

bool BinaryMathTable::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    std::vector<QVariant> data1(_inputTable->recordCount(), _number1), data2(_inputTable->recordCount(), _number2),outdata(_inputTable->recordCount(), rUNDEF);
    if ( _column1 != sUNDEF)
        data1 = _inputTable->column(_column1);

    if ( _column2 != sUNDEF)
        data2 = _inputTable->column(_column2);

    auto iter1 = data1.begin();
    auto iter2 = data2.begin();
    std::for_each(outdata.begin(), outdata.end(),[&](QVariant& v){
        double v_in1 = (*iter1).toDouble();
        double v_in2 = (*iter2).toDouble();
        v = calc(v_in1, v_in2);
        ++iter1;
        ++iter2;
    });
    _outputTable->column(_outColumn,outdata);

    if ( _outputTable.isValid()) {
        QVariant var;
        var.setValue<ITable>(_outputTable);
        _outputTable->addDescription(_expression.toString());
        ctx->setOutput(symTable,var, _outputTable->name(),itCOLUMN,_outputTable->resource(),_outColumn);
        return true;
    }


    return false;
}

OperationImplementation::State BinaryMathTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
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
    } else{
        _column1 = parm1.value();
        def1 = _inputTable->columndefinition(_column1);
    }

    if ( hasType(parm2.valuetype(),itNUMBER)){
        _number2 = parm2.value().toDouble(&ok);
    } else{
        _column2 = parm2.value();
        def2 = _inputTable->columndefinition(_column2);
    }

    mathoperator(_expression.parm(3).value());

    NumericRange *newRange = 0;
    if ( def1.isValid() && def2.isValid())  {
        if (!def1.datadef().domain<>()->isCompatibleWith(def2.datadef().domain<>().ptr())){
            ERROR2(ERR_NOT_COMPATIBLE2,_column1, _column2);
            return sPREPAREFAILED;
        }
        if (!(hasType(def1.datadef().domain<>()->valueType(),itNUMBER) && hasType(def2.datadef().domain<>()->valueType(), itNUMBER))){
            ERROR2(ERR_NOT_COMPATIBLE2,_column1, _column2);
            return sPREPAREFAILED;
        }
        newRange = constructRangeFrom(def1.datadef().range<NumericRange>(), def2.datadef().range<NumericRange>());


    } else {
        ColumnDefinition def = def1.isValid() ? def1 : def2;
        double number = _number1 == rUNDEF ? _number2 : _number1;
        if (!hasType(def.datadef().domain<>()->valueType(),itNUMBER)){
            ERROR2(ERR_NOT_COMPATIBLE2,_column1, TR("numeric"));
            return sPREPAREFAILED;
        }
        if ( number == 0 && _operator == otDIV){
            ERROR2(ERR_ILLEGAL_VALUE_2,"division","0");
            return sPREPAREFAILED;
        }
        newRange = constructRangeFrom(def.datadef().range<NumericRange>(), number);
    }
    _outColumn = "column_" + QString::number(_inputTable->columnCount());

    QString outName = _expression.parm(0, false).value();
    if ( table != outName) {
        if (outName == sUNDEF){
            IFlatTable  ftable;
            ftable.prepare();
            _outputTable = ftable;
        } else if(!_outputTable.prepare(outName)) {// output table doesnt need to exists
            _outputTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outName), _inputTable->ilwisType());
            _outputTable->name(outName);
        }
    } else
        _outputTable = _inputTable;

    int index = _outputTable->columnIndex(_outColumn);
    IDomain dom;
    dom.prepare("value");
    ColumnDefinition coldef(_outColumn,dom, index == iUNDEF ? _outputTable->columnCount() : index);

    coldef.datadef().range(newRange);
    if ( index == iUNDEF)
        _outputTable->addColumn(coldef);
    else
        _outputTable->columndefinition(index) = coldef;

    return sPREPARED;
}

quint64 BinaryMathTable::createMetadata()
{
    OperationResource operation({"ilwis://operations/binarymathtable"});
    operation.setSyntax("binarymathtable(input-table, !column-name|number,!column-name|number, !add|subtract|divide|times|mod)");
    operation.setDescription(TR("generates a new numerical table based on the operation, applied to all the cells of a column"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itTABLE, TR("input table"),TR("input table from which the input column will be chosen"));
    operation.addInParameter(1,itSTRING | itNUMBER, TR("input column or number"),TR("column with a numerical domain or number"));
    operation.addInParameter(2,itSTRING | itNUMBER , TR("input column or number"),TR("column with a numerical domain or number"));
    operation.addInParameter(3,itSTRING, TR("operator"),TR("operator (add, subtract,divide, multiply) applied to the other 2 input operators"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOLUMN, TR("output column"));
    operation.setKeywords("table,column,numeric,math");

    mastercatalog()->addItems({operation});
    return operation.id();
}



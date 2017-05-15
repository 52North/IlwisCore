#include <functional>
#include <future>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "containerstatistics.h"
#include "domain.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "numericoperation.h"
#include "tranquilizer.h"
#include "columnunaryoperation.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(ColumnUnaryOperation)

ColumnUnaryOperation::ColumnUnaryOperation()
{

}

ColumnUnaryOperation::ColumnUnaryOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : NumericOperation(metaid, expr),
    _method(NumericStatistics::pSUM)
{

}

OperationImplementation *ColumnUnaryOperation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ColumnUnaryOperation(metaid, expr);
}

NumericStatistics::PropertySets ColumnUnaryOperation::toMethod(const QString& nm) {
    QString mname = nm.toLower();
    if ( mname == "avg")
        return NumericStatistics::pMEAN;
    else if ( mname == "min")
        return NumericStatistics::pMIN;
    else if ( mname == "max")
        return NumericStatistics::pMAX;
    else if ( mname == "med")
        return NumericStatistics::pMEDIAN;
    else if ( mname == "pred")
        return NumericStatistics::pPREDOMINANT;
    else if ( mname == "std")
        return NumericStatistics::pSTDEV;
    else if ( mname == "sum")
        return NumericStatistics::pSUM;

    return NumericStatistics::pLAST;
}
bool ColumnUnaryOperation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    std::vector<QVariant> data(_inputTable->recordCount(), _number1),outdata(1, rUNDEF);
    if ( _column1 != sUNDEF)
        data = _inputTable->column(_column1);

    auto iter1 = data.begin();
    std::vector<double> values(data.size());
    int  i=0;
    for(auto v : data){
        values[i++] = v.toDouble();
    }
    NumericStatistics stats;
    stats.calculate(values.begin(), values.end(),_method);
    double outputData = stats[_method];
    std::for_each(outdata.begin(), outdata.end(),[&](QVariant& v){
        v = outputData;
        ++iter1;
    });

    _outputTable->column(_outColumn,outdata);
    if ( _outputTable.isValid()){
        QVariant var;
        var.setValue<ITable>(_outputTable);
        logOperation(_outputTable, _expression);
        ctx->setOutput(symTable,var, _outputTable->name(),itTABLE,_outputTable->resource(),_outColumn);
        return true;
    }
    return false;
}

Ilwis::OperationImplementation::State ColumnUnaryOperation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString table = _expression.parm(0).value();
    if (!_inputTable.prepare(table)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,table,"");
        return sPREPAREFAILED;
    }
    const Parameter& parm1 = _expression.parm(1);
    bool ok = true;
    ColumnDefinition def1;
    if ( hasType(parm1.valuetype(),itNUMBER)){
        _number1 = parm1.value().toDouble(&ok);
    }
    else{
        _column1 = parm1.value();
        def1 = _inputTable->columndefinition(_column1);
    }
    if (!(hasType(def1.datadef().domain<>()->valueType(),itNUMBER))){
        ERROR2(ERR_NOT_COMPATIBLE2, TR("column"), _expression.name());
        return sPREPAREFAILED;
    }

    if ( _expression.parameterCount() < 4){
        _outColumn = _expression.parm(2).value();
    }else{
        _outColumn = _expression.parm(3).value();
    }
    _method = toMethod(_expression.parm(2).value());
    if ( _method == NumericStatistics::pLAST) {
        ERROR2(ERR_ILLEGAL_VALUE_2, "parameter value", " aggregation method");
        return sPREPAREFAILED;
    }

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
    NumericRange *newRange = 0;

    int index = _outputTable->columnIndex(_outColumn);

    IDomain dom;
    dom.prepare("value");
    ColumnDefinition coldef(_outColumn,dom, index == iUNDEF ? _outputTable->columnCount() : index);
    newRange = constructRangeFrom(def1.datadef().range<NumericRange>(), _number1);
    coldef.datadef().range(newRange);
    if ( index == iUNDEF)
        _outputTable->addColumn(coldef);
    else
        _outputTable->columndefinition(index) = coldef;
    return sPREPARED;
}

quint64 ColumnUnaryOperation::createMetadata()
{
    OperationResource operation({"ilwis://operations/columnunaryoperation"});
    operation.setLongName("Column Unary Operation");
    operation.setSyntax("columnunaryoperation(input-table, column-name|number, Avg|Max|Med|Min|Pred|Std|Sum[,output-column-name])");
    operation.setDescription(TR("generates a new numerical table based on the operation, applied to all the cells of a column"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itTABLE, TR("input table-column"),TR("input table-column from which the input column will be chosen"));
    operation.addInParameter(1,itSTRING | itNUMBER, TR("input column name or number"),TR("column with a numerical domain or number"));    
    operation.addInParameter(2,itSTRING, TR("operator"),TR("operator (Average, Maximum,Median, Product, Standard Deviation, Sum) applied to the column"));
    operation.addInParameter(3,itSTRING , TR("output column"),TR("output column with a numeric domain. It is optional"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE, TR("output table"));
    operation.setKeywords("table,operation,statistics,numeric");

    mastercatalog()->addItems({operation});
    return operation.id();
}



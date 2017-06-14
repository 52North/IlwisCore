#include <QString>
#include <QStringList>
#include <functional>
#include <future>
#include <memory>
#include <QHash>
#include "coverage.h"
#include "table.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "aggregatetable.h"


using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(AggregateTable)

AggregateTable::AggregateTable()
{

}

AggregateTable::AggregateTable(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

QVariant AggregateTable::stringCase(IlwisTypes tp, const std::vector<int>& records, int c){
    if ( _aggregationMehtod == "sum" ||
         _aggregationMehtod == "average" ||
         _aggregationMehtod == "minimum" ||
         _aggregationMehtod == "maximum")
        return hasType(tp, itTEXTDOMAIN) ? QVariant(sUNDEF) : QVariant((const int)iUNDEF);

    std::vector<QVariant> values = _inputTable->column(c);
    std::map<QVariant, int> ordered;
    QVariant result;
    for(int rec : records){
        QVariant v = values[rec];
        if ( _aggregationMehtod == "predominant"){
            ordered[v]+=1;
        }
        else if ( _aggregationMehtod == "first"){
            if ( !result.isValid())
                result = v;
        } else if ( _aggregationMehtod == "last"){
            result = v;
        }
    }
    if ( _aggregationMehtod == "predominant"){
        QVariant current;
        double total = -1e308;
        for(auto elem : ordered){
            if ( elem.second > total){
                total = elem.second;
                current = elem.first;
            }
        }
        result = current;
    }
    return result;
}

QVariant AggregateTable::numericCase(const std::vector<int>& records, int c)
{
    double num = _aggregationMehtod == "minimum" ? 1e308 : rUNDEF;
    std::map<double, int> ordered;

    std::vector<QVariant> values = _inputTable->column(c);
    int recordsDone = 0;
    for(int rec : records){
        double v = values[rec].toDouble();
        if ( isNumericalUndef(v))
            continue;
        else if ( _aggregationMehtod == "sum" || _aggregationMehtod == "average"){
            num = (num == rUNDEF ? v : num + v);
        }
        else if ( _aggregationMehtod == "predominant"){
            ordered[v]+=1;
        }
        else if ( _aggregationMehtod == "first"){
            if ( num == rUNDEF)
                num = v;
        } else if ( _aggregationMehtod == "last"){
            num = v;
        } else if ( _aggregationMehtod == "minimum"){
            if ( v != rUNDEF)
                num = std::min(v, num);
        } else if ( _aggregationMehtod == "maximum"){
            if ( v != rUNDEF)
                num = std::max(v, num);
        }
        ++recordsDone;
    }
    if ( _aggregationMehtod == "sum" ||
         _aggregationMehtod == "first" ||
         _aggregationMehtod == "last" ||
         _aggregationMehtod == "maximum" ||
         _aggregationMehtod == "minimum"){
        return num;
    }
    else if ( _aggregationMehtod == "average"){
        return recordsDone > 0 ? num / recordsDone : rUNDEF;
    }
    else if ( _aggregationMehtod == "predominant"){
        double current = rUNDEF;
        double total = -1e308;
        for(auto elem : ordered){
            if ( elem.second > total){
                total = elem.second;
                current = elem.first;
            }
        }
         return current;
    }
    return QVariant();
}

bool AggregateTable::execute(ExecutionContext *ctx, SymbolTable &symTable)


{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    std::map<QString, std::vector<int>> recordsByKey;
    std::vector<QVariant> values = _inputTable->column(_aggregateColumn);
    std::map<QString, std::vector<QVariant>> tableValues;
    std::vector<IlwisTypes> types;
    for(int c = 0; c < _inputTable->columnCount(); ++c){
        types.push_back(_inputTable->columndefinition(c).datadef().domain()->ilwisType());
    }

    for(int r = 0; r < _inputTable->recordCount(); ++r){
        recordsByKey[_inputTable->cell(_aggregateColumn,r).toString()].push_back(r);
    }
    initialize(recordsByKey.size());
    int count = 0;

    for(auto iter = recordsByKey.begin(); iter != recordsByKey.end();++iter){
        std::vector<int>& records = (*iter).second;
        if (tableValues[(*iter).first].size() == 0)
            tableValues[(*iter).first].resize(_inputTable->columnCount()) ;

        updateTranquilizer(count++, 5);
        for(int c = 0; c < _inputTable->columnCount(); ++c){
            if ( c == _aggregateColumn){
                if ( types[c] == itTEXTDOMAIN)
                    tableValues[(*iter).first][c] = (*iter).first;
                else
                    tableValues[(*iter).first][c] = (*iter).first.toDouble();
            }
            else if ( hasType(types[c], itNUMERICDOMAIN)){
                tableValues[(*iter).first][c] = numericCase(records, c);
            }
            else if ( hasType(types[c], itITEMDOMAIN | itTEXTDOMAIN)){
                tableValues[(*iter).first][c] = stringCase(types[c], records, c);
            }
        }
    }
    for(auto& row : tableValues){
        _outputTable->record(NEW_RECORD, row.second);
    }

    QVariant value;
    value.setValue<ITable>(_outputTable);
    logOperation(_outputTable, _expression);
    ctx->setOutput(symTable,value,_outputTable->name(), itTABLE, _outputTable->resource() );

    return true;
}

Ilwis::OperationImplementation *AggregateTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new  AggregateTable(metaid, expr)   ;
}

Ilwis::OperationImplementation::State AggregateTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    OperationHelper::check([&] ()->bool { return _inputTable.prepare(_expression.input<QString>(0), itTABLE); },
    {ERR_COULD_NOT_LOAD_2,_expression.input<QString>(0), "" } );

    std::vector<QString> methods={"sum" ,"average" ,"maximum" ,"minimum" ,"predominant","last","first"};
    QString aggregateColumn = _expression.input<QString>(1);
    if ( (_aggregateColumn = _inputTable->columnIndex(aggregateColumn)) == iUNDEF){
        kernel()->issues()->log(TR("Non existing aggregation column ") + _aggregateColumn);
        return sPREPAREFAILED;
    }
    _aggregationMehtod = _expression.input<QString>(2);
    if (std::find(methods.begin(), methods.end(),_aggregationMehtod) == methods.end()){
        kernel()->issues()->log(TR("Invalid aggregation method ") + _aggregationMehtod);
        return sPREPAREFAILED;
    }

    _outputTable.prepare();
    for(int c = 0; c < _inputTable->columnCount(); ++c){
        _outputTable->addColumn( _inputTable->columndefinition(c));
    }

    return sPREPARED;
}

quint64 AggregateTable::createMetadata()
{
    OperationResource operation({"ilwis://operations/groupby"});
    operation.setSyntax("groupby(inputtable,aggregationColumn, aggregationMethod=!sum|average|maximum|minimum|predominant|last|first)");
    operation.setLongName("Group By");
    operation.setDescription(TR("Aggregates a table based on a certain column and aggregates each column values"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itTABLE , TR("input table"),TR("input table"));
    operation.addInParameter(1,itSTRING, TR("group column"), TR("Column which is used for the aggregation"), OperationResource::ueCOMBO);
    operation.addInParameter(2,itSTRING, TR("aggreation method"), TR("method how the values are aggregated"));
    operation.addValidation(0,1,"columns");
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE , TR("aggregated table"),TR("new table where the columns have been aggregated"));
    operation.setKeywords("table,aggregate,column");

    mastercatalog()->addItems({operation});
    return operation.id();
}

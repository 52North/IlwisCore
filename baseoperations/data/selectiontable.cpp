#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwisobject.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "tablemerger.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "tranquilizer.h"
#include "selectiontable.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SelectionTable)

Ilwis::OperationImplementation *SelectionTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SelectionTable(metaid, expr);
}

SelectionTable::SelectionTable()
{
}

SelectionTable::SelectionTable(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

SelectionTable::~SelectionTable()
{

}

bool SelectionTable::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    QString addInfo;
    quint32 start = 0;
    quint32 stop = _inputTable->recordCount();
    if ( _records.size() != 0) {
        start = _records[0].first;
        stop = _records[0].second;
    }
    for(const auto& colname : _columns) {
        std::vector<QVariant> coldata = _inputTable->column(colname,start, stop);
        _outputTable->column(colname, coldata);
        if ( addInfo !="")
            addInfo += ",";
        addInfo += colname;
    }
    if ( _outputTable.isValid()) {
        QVariant var;
        var.setValue<ITable>(_outputTable);
        logOperation(_outputTable, _expression);
        ctx->setOutput(symTable,var, _outputTable->name(),itTABLE,_outputTable->resource(),addInfo);
        return true;
    }
    return false;
}

Ilwis::OperationImplementation::State SelectionTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString table = _expression.parm(0).value();
    if (!_inputTable.prepare(table)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,table,"");
        return sPREPAREFAILED;
    }
    QString selectionExpression = _expression.parm(1).value();
    QString outName = _expression.parm(0, false).value();
    if(!outName.contains(QRegExp("\\\\|/")) && !outName.contains("code=")) {
        _outputTable.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outName), _inputTable->ilwisType());
        _outputTable->name(outName);

    }else {
        _outputTable.prepare(outName,_inputTable->ilwisType());
    }
    selectionExpression = selectionExpression.remove('\"');
    int index;
    if ( (index=selectionExpression.indexOf("attribute="))!=-1){
        _columns.push_back(selectionExpression.mid(10));
    } else if ((index=selectionExpression.indexOf("recordrange="))!= -1){
        for(int  i = 0; i < _inputTable->columnCount(); ++i){
            _columns.push_back(_inputTable->columndefinition(i).name());
        }
        QStringList parts = selectionExpression.mid(12).split(",");
        for(int i=0; i < parts.size() / 2; i+=2) {
            _records.push_back(std::pair<quint32, quint32>(parts[i].toUInt(), parts[i+1].toUInt()));
        }
    }
    TableMerger merger;
    bool ok = merger.mergeMetadataTables(_outputTable, _inputTable, _columns);
    if ( _records.size() != 0) {
        quint32 recs= 0;
        for(const auto& rng : _records) {
            recs += rng.second - rng.first;
        }
        _outputTable->recordCount(recs);
    }
    if (!ok)
        return sPREPAREFAILED;

    return sPREPARED;
}

quint64 SelectionTable::createMetadata()
{
    OperationResource operation({"ilwis://operations/selection"});
    operation.setSyntax("selection(table,selection-definition)");
    operation.setDescription(TR("the operation select one or more column and puts them in smaller table"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itTABLE , TR("input table"),TR("input table with a domain as specified by the selection"));
    operation.addInParameter(1,itSTRING, TR("selection-definition"), TR("Selection can either be an (set of)attribute(s) or record definition"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itTABLE , TR("Selection"),TR("table were the selection has been applied"));

    mastercatalog()->addItems({operation});
    return operation.id();

}



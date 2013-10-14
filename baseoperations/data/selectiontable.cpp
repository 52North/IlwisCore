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
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "tranquilizer.h"
#include "selectiontable.h"

using namespace Ilwis;
using namespace BaseOperations;

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
    return true;
}

Ilwis::OperationImplementation::State SelectionTable::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString table = _expression.parm(0).value();
    if (!_inputTable.prepare(table)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,table,"");
        return sPREPAREFAILED;
    }
    QString columnExpression = _expression.parm(1).value();
    if ( columnExpression.indexOf(",") == -1){

    }
    return sPREPARED;
}

quint64 SelectionTable::createMetadata()
{
    QString url = QString("ilwis://operations/selection");
    Resource resource(QUrl(url), itOPERATIONMETADATA);
    resource.addProperty("namespace","ilwis");
    resource.addProperty("longname","selection");
    resource.addProperty("syntax","selection(table,selection-definition)");
    resource.addProperty("description",TR("the operation select one or more column and puts them in smaller table"));
    resource.addProperty("inparameters","2");
    resource.addProperty("pin_1_type", itTABLE);
    resource.addProperty("pin_1_name", TR("input table"));
    resource.addProperty("pin_1_desc",TR("input table with a domain as specified by the selection"));
    resource.addProperty("pin_2_type", itSTRING);
    resource.addProperty("pin_2_name", TR("selection-definition"));
    resource.addProperty("pin_2_desc",TR("Selection can either be an (set of)attribute(s) or record definition"));
    resource.addProperty("pout_1_type", itTABLE);
    resource.addProperty("pout_1_name", TR("Selection"));
    resource.addProperty("pout_1_desc",TR("table were the selection has been applied"));
    resource.prepare();
    url += "=" + QString::number(resource.id());
    resource.setUrl(url);

    mastercatalog()->addItems({resource});
    return resource.id();
}



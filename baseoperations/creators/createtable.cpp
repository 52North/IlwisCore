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
#include "createtable.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateTable)

CreateTable::CreateTable()
{

}

CreateTable::CreateTable(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateTable::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    ITable tbl;
    IOOptions options({"create", true}) ;
    tbl.prepare(_outputName, options);
    ctx->_additionalInfo["outputisinput"] = true;
    if ( tbl.isValid()){
        setOutput(tbl,ctx, symTable);
        return true;
    }

    return false;
}

Ilwis::OperationImplementation *CreateTable::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateTable(metaid, expr);
}

Ilwis::OperationImplementation::State CreateTable::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    _outputName = _expression.input<QString>(0);
    if ( _outputName.indexOf("file://") == -1){
        QString workingfolder = context()->workingCatalog()->resource().url().toString();
        if ( workingfolder.indexOf("file://") != -1){
            _outputName = workingfolder + "/" + _outputName;
        }else {
            _outputName = INTERNAL_CATALOG + "/" + _outputName;
        }
        int index = _outputName.lastIndexOf("/");
        QString namePart = _outputName.mid(index+1);
        index = namePart.lastIndexOf(".");
        if ( index == -1)
            _outputName += ".ilwis";
    }

    return sPREPARED;
}

quint64 CreateTable::createMetadata()
{
    OperationResource resource({"ilwis://operations/createtable"});
    resource.setLongName("Create Table");
    resource.setSyntax("createtable())");
    resource.setInParameterCount({1});
    resource.addInParameter(0, itSTRING,TR("Table name"), TR("New tables name. This will overrule any name given in the output field"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itTABLE, TR("new table"), TR("The newly created empty table"));
    resource.setKeywords("create, table, workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

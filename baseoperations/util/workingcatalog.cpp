#include <iostream>
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "workingcatalog.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(WorkingCatalog)


WorkingCatalog::WorkingCatalog()
{
}


WorkingCatalog::WorkingCatalog(quint64 metaid, const Ilwis::OperationExpression &expr) :  OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *WorkingCatalog::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new WorkingCatalog(metaid, expr);
}

bool WorkingCatalog::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
   logOperation( _expression);
   context()->setWorkingCatalog(ICatalog(_location.toString()));
   mastercatalog()->addContainer(_location);

    return true;
}

Ilwis::OperationImplementation::State WorkingCatalog::prepare(ExecutionContext *ctx, const Ilwis::SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString location =  _expression.input<QString>(0);
    QUrl url( location);
    if ( !url.isValid() || url.isEmpty() || url.isRelative()){
        ERROR2(ERR_ILLEGAL_VALUE_2,"Url",location);
        return sPREPAREFAILED;
    }
    _location = url;
    return sPREPARED;
}

quint64 WorkingCatalog::createMetadata()
{
    OperationResource operation({"ilwis://operations/setworkingcatalog"});
    operation.setLongName(TR("Set Working Catalog"));
    operation.setDescription(TR("The working catalog is the only location were data maybe used without full url as path. It is a convenience system for creating more readable expressions"));
    operation.setSyntax("setworkingcatalog(url)");
    operation.setInParameterCount({1});
    operation.addInParameter(0,itSTRING , TR("Location working catalog"),TR("The default location were to look for data as an url"));
    operation.setOutParameterCount({0});
    operation.setKeywords("data,workflow");

    mastercatalog()->addItems({operation});
    return operation.id();
}

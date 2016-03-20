#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "uicontextmodel.h"
#include "drawers/draweroperation.h"
#include "drawers/drawerfactory.h"
#include "models/layermanager.h"
#include "drawers/drawerinterface.h"
#include "cleardrawers.h"
#include "../rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(ClearDrawers)

ClearDrawers::ClearDrawers()
{
}

ClearDrawers::~ClearDrawers()
{

}

ClearDrawers::ClearDrawers(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool ClearDrawers::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

    rootdrawer->prepare(DrawerInterface::ptRESET, IOOptions());

    return true;
}

Ilwis::OperationImplementation *ClearDrawers::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ClearDrawers(metaid, expr);
}

Ilwis::OperationImplementation::State ClearDrawers::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto iter = ctx->_additionalInfo.find("rootdrawer");
    if ( iter == ctx->_additionalInfo.end())
        return sPREPAREFAILED;

     _rootDrawer =  (DrawerInterface *)  (*iter).second.value<void *>();


    return sPREPARED;
}

quint64 ClearDrawers::createMetadata()
{
    OperationResource operation({"ilwis://operations/cleardrawers"});
    operation.setSyntax("clearDrawers(viewerid)");
    operation.setDescription(TR("removes all the drawers and resets the coordinatesystem"));
    operation.setInParameterCount({1});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}






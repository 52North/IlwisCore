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
#include "../layerdrawer.h"
#include "removedrawer.h"
#include "../rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(RemoveDrawer)

RemoveDrawer::RemoveDrawer()
{
}

RemoveDrawer::~RemoveDrawer()
{

}

RemoveDrawer::RemoveDrawer(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool RemoveDrawer::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

    if ( _drawerOrder == iUNDEF)
        rootdrawer->removeDrawer(_drawercode,true);
    else
        rootdrawer->removeDrawer(_drawerOrder,_drawerType);

    return true;
}

Ilwis::OperationImplementation *RemoveDrawer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RemoveDrawer(metaid, expr);
}

Ilwis::OperationImplementation::State RemoveDrawer::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto iter = ctx->_additionalInfo.find("rootdrawer");
    if ( iter == ctx->_additionalInfo.end())
        return sPREPAREFAILED;
    _rootDrawer =  (DrawerInterface *)  (*iter).second.value<void *>();
    bool ok;
    _viewid = _expression.parm(0).value().toULongLong(&ok);
    if ( !ok){
        ERROR3(ERR_ILLEGAL_PARM_3,"view id", _expression.parm(0).value(), _expression.toString());
        return sPREPAREFAILED;
    }
    _drawerOrder = _expression.input<quint32>(1,ok);
    if ( !ok){
        _drawercode = _expression.input<QString>(1);
        _drawerOrder = iUNDEF;
    }

    QString drwtype = _expression.input<QString>(2).toLower();

    _drawerType = DrawerInterface::dtDONTCARE;

    if ( drwtype == "pre"){
        _drawerType = DrawerInterface::dtPRE;
    } else if ( drwtype == "main"){
        _drawerType = DrawerInterface::dtMAIN;
    }   if ( drwtype == "post"){
        _drawerType = DrawerInterface::dtPOST;
    }

    return sPREPARED;
}

quint64 RemoveDrawer::createMetadata()
{
    OperationResource operation({"ilwis://operations/removedrawer"});
    operation.setSyntax("removedrawer(viewid, drawercode)");
    operation.setDescription(TR("changes the view extent"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("Drawer code "), TR("unique code identifying the drawer"));
    operation.addInParameter(2,itSTRING , TR("Drawer type "), TR("pre, main or post drawer"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}




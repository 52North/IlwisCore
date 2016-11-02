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
#include "showcoverage.h"

#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(ShowCoverage)

ShowCoverage::ShowCoverage()
{

}

Ilwis::Geodrawer::ShowCoverage::ShowCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool ShowCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    qint64 id = uicontext()->addMapPanel("itemid=" + QString::number(_id),_side, _url);
    if ( id == -1)
        return false;

    QVariant v1;
    v1.setValue(id);
    ctx->setOutput(symTable, QVariant(v1), sUNDEF, itINTEGER, Resource());


    return true;
}

Ilwis::OperationImplementation *ShowCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new ShowCoverage(metaid, expr);
}

Ilwis::OperationImplementation::State ShowCoverage::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    _url = _expression.input<QString>(0);
    ICoverage cov(_url, itCOVERAGE,{"mustexist", true});
    if ( !cov.isValid()){
        kernel()->issues()->log(_url + TR(" is not a valid coverage"));
        return sPREPAREFAILED ;
    }
    _id = cov->id();
    _side = _expression.input<QString>(1).toLower();
    if ( !(_side == "left" || _side == "right" || _side == "other")){
        kernel()->issues()->log(_side + TR(" is not a valid side type"));
        return sPREPAREFAILED ;
    }

    return sPREPARED;
}

quint64 ShowCoverage::createMetadata()
{
    OperationResource operation({"ilwis://operations/showcoverage"});
    operation.setSyntax("showcoverage(coverageurl,!other|left|right)");
    operation.setDescription(TR("Creates a new mappanel with the indicated coverage"));
    operation.setInParameterCount({2});
    operation.addInParameter(0,itCOVERAGE , TR("Coverage url"),TR("The url of the coverage to be displayed"));
    operation.addInParameter(1,itSTRING , TR("Panel side"),TR("Which side should the panel open, left, right or at the other side of the active panel"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itINTEGER , TR("View id"),TR("Id of the view that is opened. The id can be used in subsequent visualization operations"));
    operation.setKeywords("visualization");
    operation.addProperty("runinmainthread",true); // this operation invokes a qml method so it must run in the main thread

    mastercatalog()->addItems({operation});
    return operation.id();
}


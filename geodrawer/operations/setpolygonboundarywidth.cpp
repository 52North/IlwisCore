#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "drawers/drawerinterface.h"
#include "drawers/draweroperation.h"
#include "../rootdrawer.h"
#include "setpolygonboundarywidth.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(SetPolygonBoundaryWidth)

SetPolygonBoundaryWidth::SetPolygonBoundaryWidth()
{

}

SetPolygonBoundaryWidth::~SetPolygonBoundaryWidth()
{

}

SetPolygonBoundaryWidth::SetPolygonBoundaryWidth(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetPolygonBoundaryWidth::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

        if ( _index != iUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_index, _type);
            drawer->setAttribute("boundarywidth", _boundarywidth);
        }else if ( _code != sUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_code, _type)    ;
            drawer->setAttribute("boundarywidth", _boundarywidth);
        }
        return true;
    }catch(const VisualizationError& err){
    }
    return false;
}

Ilwis::OperationImplementation *SetPolygonBoundaryWidth::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetPolygonBoundaryWidth(metaid, expr)    ;
}

Ilwis::OperationImplementation::State SetPolygonBoundaryWidth::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( (_rootDrawer = getRootDrawer()) == 0){
        return sPREPAREFAILED;
    }

    QString type = _expression.parameterCount() == 3 ? "main" : _expression.input<QString>(3);
    bool ok;
    int index = _expression.parm(1).value().toInt(&ok);
    if ( ok){
        if ( type == "main" && index >= 0 && index < _rootDrawer->attribute("maindrawercount").toInt()){
            _index = index;
        }else {
            _index = index;
        }
    }else {
        _code = _expression.input<QString>(1);
    }

    if ( type == "main")
        _type = DrawerInterface::dtMAIN;
    else if ( type == "post")
        _type = DrawerInterface::dtPOST;
    else if ( type == "pre"){
        _type = DrawerInterface::dtPRE;
    }else{
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"), type);
        return sPREPAREFAILED;
    }

    _boundarywidth = _expression.input<double>(2);

    return sPREPARED;
}

quint64 SetPolygonBoundaryWidth::createMetadata()
{
    OperationResource operation({"ilwis://operations/setpolygonboundarywidth"});
    operation.setSyntax("SetPolygonBoundaryWidth(viewid, drawername-index, width[,type])");
    operation.setDescription(TR("sets or resets the visibility of a layer"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view from which the polygon boundary width should be changed"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used. In the case of pre or post drawers the index is the order number"));
    operation.addInParameter(2,itDOUBLE , TR("width"));
    operation.addOptionalInParameter(3,itSTRING , TR("drawer type"), TR("the type fo drawer, pre, main, or post. The default is main"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}




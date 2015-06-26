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
#include "setpolygonboundarycolor.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(SetPolygonBoundaryColor)

SetPolygonBoundaryColor::SetPolygonBoundaryColor()
{

}

SetPolygonBoundaryColor::~SetPolygonBoundaryColor()
{

}

SetPolygonBoundaryColor::SetPolygonBoundaryColor(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetPolygonBoundaryColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

        if ( _index != iUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_index, _type);
            drawer->setAttribute("boundarycolor", QVariant(_boundaryColor));
        }else if ( _code != sUNDEF){
            UPDrawer& drawer = rootdrawer->drawer(_code, _type)    ;
            drawer->setAttribute("boundarycolor", QVariant(_boundaryColor));
        }
        return true;
    }catch(const VisualizationError& err){
    }
    return false;
}

Ilwis::OperationImplementation *SetPolygonBoundaryColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetPolygonBoundaryColor(metaid, expr)    ;
}

Ilwis::OperationImplementation::State SetPolygonBoundaryColor::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( (_rootDrawer = getRootDrawer()) == 0){
        return sPREPAREFAILED;
    }

    QString type = _expression.parameterCount() == 3 ? "main" : _expression.input<QString>(3);
    bool ok;
    int index = _expression.parm(1).value().toInt(&ok);
    if ( ok){
        if ( type == "main" && _index >= 0 && _index < _rootDrawer->attribute("maindrawercount").toInt()){
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

    _boundaryColor = QColor(_expression.input<QString>(2));
    if (!_boundaryColor.isValid()){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("color value"), _expression.input<QString>(2));
        return sPREPAREFAILED;
    }

    return sPREPARED;
}

quint64 SetPolygonBoundaryColor::createMetadata()
{
    OperationResource operation({"ilwis://operations/setpolygonboundarycolor"});
    operation.setSyntax("setpolygonboundarycolor(viewid, drawername-index, color-string[,type])");
    operation.setDescription(TR("sets or resets the visibility of a layer"));
    operation.setInParameterCount({3,4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used. In the case of pre or post drawers the index is the order number"));
    operation.addInParameter(2,itSTRING , TR("color name"), TR("either an svg color name or a hex based color code (e.g #ff1145)"));
    operation.addInParameter(3,itSTRING , TR("drawer type"), TR("optional,the type fo drawer, pre, main, or post. The default is main"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}




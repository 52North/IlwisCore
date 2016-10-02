#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "drawers/drawerinterface.h"
#include "drawers/draweroperation.h"
#include "models/layermanager.h"
#include "setlayeropacity.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(SetLayerOpacity)

SetLayerOpacity::SetLayerOpacity()
{

}

SetLayerOpacity::~SetLayerOpacity()
{

}

SetLayerOpacity::SetLayerOpacity(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetLayerOpacity::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    _drawer->setAttribute("opacity", _alpha);
    _rootDrawer->redraw();
    return true;
}

Ilwis::OperationImplementation *SetLayerOpacity::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetLayerOpacity(metaid,expr)    ;
}

Ilwis::OperationImplementation::State SetLayerOpacity::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( (_rootDrawer = getRootDrawer()) == 0){
        return sPREPAREFAILED;
    }

    quint64 viewerid = _expression.input<quint32>(0);
    LayersViewCommandInterface *viewer = uicontext()->viewer(viewerid);
    if (!viewer)
        return sPREPAREFAILED;
    LayerManager *manager = viewer->layerManager();
    quint32 layerIndex = _expression.input<quint32>(1);
    CoverageLayerModel *layer = manager->layer(layerIndex);
    if (!layer)
        return sPREPAREFAILED;
    float alpha = _expression.parm(2).value().toFloat();
    if (alpha < 0 || alpha > 1)
        return sPREPAREFAILED;
    _alpha = alpha;
    _drawer = layer->drawer();

    return sPREPARED;
}

quint64 SetLayerOpacity::createMetadata()
{
    OperationResource operation({"ilwis://operations/setlayeropacity"});
    operation.setSyntax("setlayeropacity(viewid, layerindex,opacity)");
    operation.setDescription(TR("Sets the opacity ( alpha channel) of the layer indicated by the index"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itINTEGER , TR("layer index"));
    operation.addInParameter(2,itDOUBLE , TR("opacity value"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

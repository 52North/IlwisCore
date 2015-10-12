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
#include "layermanagement.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(LayerManagement)

LayerManagement::LayerManagement()
{

}

LayerManagement::~LayerManagement()
{

}

LayerManagement::LayerManagement(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid,expr)
{

}

bool LayerManagement::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    LayersViewCommandInterface *viewer = uicontext()->viewer(_viewerid);
    if (!viewer)
        return sPREPAREFAILED;
    LayerManager *manager = viewer->layerManager();
    int currentIndex = iUNDEF;
    QObject *obj = uicontext()->rootObject()->findChild<QObject *>(manager->layerListName());
    if ( obj){
        QVariant vIndex = obj->property("currentIndex");
        if ( _action == LayerManager::lmUP || _action == LayerManager::lmREMOVE)
            currentIndex = vIndex.toInt() -1;
        else
           currentIndex = vIndex.toInt() + 1;
    }
    manager->moveLayer(_index,_action);
    manager->refresh();

    QVariant count = _rootDrawer->attribute("maindrawercount");
    int maxcount = count.toInt();
    //drawers are in reversed order in the draw stack; drawer 0 is the bottom layer, maxcount-1, the top layer while in the manager it is the other way around
    _rootDrawer->execute(_command,{std::pair<QString, QVariant>("index", maxcount - _index - 1)});

    if ( obj){
        obj->setProperty("currentIndex",currentIndex);
    }
    return true;

    } catch(const VisualizationError& err){
    }
    return false;
}

Ilwis::OperationImplementation *LayerManagement::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new  LayerManagement(metaid, expr)   ;
}

Ilwis::OperationImplementation::State LayerManagement::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto iter = ctx->_additionalInfo.find("rootdrawer");
    if ( iter == ctx->_additionalInfo.end())
        return sPREPAREFAILED;
    _rootDrawer =  (DrawerInterface *)  (*iter).second.value<void *>();
    _viewerid = _expression.input<quint32>(0);
    bool ok;
    int index = _expression.parm(1).value().toInt(&ok);
    if ( ok){
        QVariant count = _rootDrawer->attribute("maindrawercount");
        int maxcount = count.toInt();
        if ( index >= 0 && index < maxcount){
            _index = index;
        }
    }else {
        QString code = _expression.input<QString>(1);
        QVariant var = _rootDrawer->execute("code2index", {std::pair<QString, QVariant>("code", code)});
        if (!var.isValid()){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"), code);
            return sPREPAREFAILED;
        }
        _index = var.toInt();

    }
    _command = _expression.input<QString>(2).toLower();
    if ( !(_command == "layerup" || _command == "layerdown" || _command == "layerremove")){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("parameter"), _command);
        return sPREPAREFAILED;
    }
    if ( _command == "layerup"){
        _action = LayerManager::lmUP;
    }
    if ( _command == "layerdown"){
        _action = LayerManager::lmDOWN;
    }
    if ( _command == "layerremove"){
        _action = LayerManager::lmREMOVE;
    }

    return sPREPARED;
}

quint64 LayerManagement::createMetadata()
{
    OperationResource operation({"ilwis://operations/layermanagement"});
    operation.setSyntax("layermanagement(viewid, drawername-index, direction=up | !down|remove)");
    operation.setDescription(TR("changes the position of a layer within the layerview"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used. In the case of pre or post drawers the index is the order number"));
    operation.addInParameter(2,itSTRING , TR("action"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}


#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "symboltable.h"
#include "representation.h"
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
#include "setrepresentation.h"
#include "../rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(SetRepresentation)

SetRepresentation::SetRepresentation()
{

}

SetRepresentation::SetRepresentation(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetRepresentation::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    try {
        if (_prepState == sNOTPREPARED)
            if((_prepState = prepare(ctx,symTable)) != sPREPARED)
                return false;
        QVariant var1, var2;
        // we must align both domain and representation
        var1.setValue(_representation->domain());
        if(!_rootDrawer->drawerAttribute(_layerCode,"visualattribute|domain|" + _attributeName,var1))
            return false;
        var2.setValue(_representation);
        if(!_rootDrawer->drawerAttribute(_layerCode,"visualattribute|representation|" + _attributeName,var2))
            return false;

        return true;
    }catch(const ErrorObject& ){
    }
    return false;
}

Ilwis::OperationImplementation *SetRepresentation::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetRepresentation(metaid, expr);
}

Ilwis::OperationImplementation::State SetRepresentation::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    if ( (_rootDrawer = getRootDrawer()) == 0){
        return sPREPAREFAILED;
    }

    bool ok;
    int index = _expression.parm(1).value().toInt(&ok);
    if ( ok){
        if ( index >= 0 && index < _rootDrawer->attribute("maindrawercount").toInt()){
            RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);
            _layerCode = rootdrawer->drawer(index)->name();
        }else {
            kernel()->issues()->log(TR("attributefillcolor operation :Invalid drawer index, operation not executed"));
        }
    }else {
        _layerCode = _expression.input<QString>(1);
    }


    // test if this is a existing layer code. every layer has an alpha channel, if it does not exist tha qvariant will be invalid
   QVariant alpha = _rootDrawer->attributeOfDrawer(_layerCode, "opacity");
   if ( !alpha.isValid()){
       kernel()->issues()->log(TR("attributefillcolor operation : The drawer can not be found, operation not executed"));
       return sPREPAREFAILED;
   }

    QVariant vrpr = _rootDrawer->attributeOfDrawer(_layerCode,"visualattribute|domain|" + _expression.input<QString>(2));
    if ( !vrpr.isValid()){
        kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
        return sPREPAREFAILED;
    }
    IDomain dom = vrpr.value<IDomain>();
    if ( !dom.isValid()){
       kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
       return sPREPAREFAILED;
    }
    if ( hasType(dom->ilwisType(), itITEMDOMAIN | itNUMERICDOMAIN)){
       kernel()->issues()->log(QString(TR("Attribute %1 is not a valid attribute for the display layer")).arg(_expression.input<QString>(2)));
       return sPREPAREFAILED;
    }
    _attributeName = _expression.input<QString>(2);
    QString rprTxt = _expression.input<QString>(3);
    if ( rprTxt.indexOf("://") != -1){
        if(!_representation.prepare(rprTxt, itREPRESENTATION)){
            kernel()->issues()->log(QString(TR("%1 is not a valid representation")).arg(rprTxt));
            return sPREPAREFAILED;
        }
    }else {
        QString code = "code=rpr:" + rprTxt;
        if(!_representation.prepare(code)){
            if(!_representation.prepare(rprTxt, itREPRESENTATION)){
                kernel()->issues()->log(QString(TR("%1 is not a valid representation")).arg(rprTxt));
                return sPREPAREFAILED;
            }
        }
    }
    return sPREPARED;
}

quint64 SetRepresentation::createMetadata()
{
    OperationResource operation({"ilwis://operations/setrepresentation"});
    operation.setSyntax("setrepresentation(viewid, drawername-index, attribname,representation)");
    operation.setDescription(TR("sets a representation for the rendering of an attribute of a layer"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER , TR("layer index/code"), TR("location of the layer to be used. In the case of pre or post drawers the index is the order number"));
    operation.addInParameter(2,itSTRING , TR("attribute name"));
    operation.addInParameter(3,itSTRING , TR("representation code or url"), TR("A representation either defined by a code or an url"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization,representation");

    mastercatalog()->addItems({operation});
    return operation.id();
}


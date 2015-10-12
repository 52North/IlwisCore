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
#include "representationattributefillcolor.h"
#include "../rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(RepresentationAttributeFillColor)

RepresentationAttributeFillColor::RepresentationAttributeFillColor()
{

}

Ilwis::Geodrawer::RepresentationAttributeFillColor::RepresentationAttributeFillColor(quint64 metaid, const Ilwis::OperationExpression &expr): DrawerOperation(metaid, expr)
{

}

bool Ilwis::Geodrawer::RepresentationAttributeFillColor::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

    QVariant vrpr = _rootDrawer->attributeOfDrawer(_layerCode,"visualattribute|representation|" + _attribute );
    IRepresentation rpr = vrpr.value<IRepresentation>();
    if ( hasType(rpr->domain()->ilwisType(), itITEMDOMAIN)){
        IItemDomain dom = rpr->domain().as<ItemDomain<DomainItem>>();
        auto item = dom->item(_item);
        if ( item) {
            Raw raw = item->raw();
            if ( raw != rUNDEF){
                rpr->colors()->setColor(raw,_color);
            }
            RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);
            auto& drawer = rootdrawer->drawer(_layerCode);
            drawer->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
            drawer->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
            rootdrawer->redraw();
        }
    }
    return true;
}

Ilwis::OperationImplementation *Ilwis::Geodrawer::RepresentationAttributeFillColor::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new RepresentationAttributeFillColor(metaid, expr);
}

Ilwis::OperationImplementation::State Ilwis::Geodrawer::RepresentationAttributeFillColor::prepare(ExecutionContext *ctx, const SymbolTable &)
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
    QString attr = _expression.input<QString>(2);
    // we can only change the colors of attributes with a representation that is not dependent on textdomain or id domain
    QVariant vrpr = _rootDrawer->attributeOfDrawer(_layerCode,"visualattribute|representation|" + attr );
    QString item = _expression.input<QString>(3);
    if ( vrpr.isValid()){
        IRepresentation rpr = vrpr.value<IRepresentation>();
        if ( rpr.isValid() && rpr->domain().isValid()){
            if ( hasType(rpr->domain()->ilwisType(), itITEMDOMAIN)){
                IItemDomain dom = rpr->domain().as<ItemDomain<DomainItem>>();
                _attribute = attr;
                if ( dom->contains(item)){
                    _item = item;

                }
            }
        }
    }
    if ( _attribute == sUNDEF){
        kernel()->issues()->log(QString(TR("attributefillcolor operation: Not a valid attribute : %1 (wrong domain?)").arg(attr)));
        return sPREPAREFAILED;
    }

    if ( _item == sUNDEF){
        kernel()->issues()->log(QString(TR("attributefillcolor operation: Not a valid item : %1 (wrong value?)").arg(attr)));
        return sPREPAREFAILED;
    }

    _color = QColor(_expression.input<QString>(4));
    if ( !_color.isValid()){
        kernel()->issues()->log(QString(TR("attributefillcolor operation : Not a valid (%1) hexadecimal color code")).arg(_expression.input<QString>(3)));
        return sPREPAREFAILED;
    }

    return sPREPARED;

}

quint64 Ilwis::Geodrawer::RepresentationAttributeFillColor::createMetadata()
{
    OperationResource operation({"ilwis://operations/attributefillcolor"});
    operation.setSyntax("attributefillcolor(viewid, selector=layerindex|code,attribute,itemname, color)");
    operation.setDescription(TR("changes the fill color of an representation item of an attribute"));
    operation.setInParameterCount({5});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING|itINTEGER, TR("layer code/index"),TR("The name of the drawer or the type of drawer will be selected based on this parameter"));
    operation.addInParameter(2,itSTRING , TR("attribute name"),TR("The attribute whose representation color needs to be changed"));
    operation.addInParameter(3,itSTRING , TR("item name"),TR("the item to be changed"));
    operation.addInParameter(4,itSTRING , TR("color"),TR("the new color of the representation item"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

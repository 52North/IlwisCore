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
#include "models/visualizationmanager.h"
#include "drawers/drawerinterface.h"
#include "../layerdrawer.h"
#include "adddrawer.h"

#include "../rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_OPERATION(AddDrawer)

AddDrawer::AddDrawer()
{

}

AddDrawer::~AddDrawer()
{

}

Ilwis::Geodrawer::AddDrawer::AddDrawer(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool AddDrawer::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;
    RootDrawer *rootdrawer = static_cast<RootDrawer *>(_rootDrawer);

    Geodrawer::DrawerInterface *drawer;
    if ( _coverage.isValid())    {
        LayerDrawer *ldrawer = DrawerFactory::create<LayerDrawer>(_coverage->ilwisType(), rootdrawer, rootdrawer, IOOptions());
        ldrawer->coverage(_coverage);
        if ( rootdrawer->drawerCount(Geodrawer::ComplexDrawer::dtMAIN) == 0)
            rootdrawer->coordinateSystem(_coverage->coordinateSystem());
        rootdrawer->addSpatialDrawer(ldrawer,false);
        QVariant var = qVariantFromValue((void *)ldrawer);
        ctx->addOutput(symTable,var,"layerdrawer",_coverage->ilwisType(), Resource());
    }else if ( _drawerCode != ""){
        drawer = Geodrawer::DrawerFactory::create<>(_drawerCode, rootdrawer, rootdrawer, IOOptions());
        rootdrawer->addDrawer(drawer,drawer->drawerType(),drawer->defaultOrder());
    }
    return true;
}

Ilwis::OperationImplementation *AddDrawer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AddDrawer(metaid, expr);
}

Ilwis::OperationImplementation::State AddDrawer::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    auto iter = ctx->_additionalInfo.find("rootdrawer");
    if ( iter == ctx->_additionalInfo.end())
        return sPREPAREFAILED;

    if ( _expression.parameterCount() == 4){
        QString url = _expression.input<QString>(2);
        QString tpname = _expression.input<QString>(3);
        IlwisTypes tp = IlwisObject::name2Type(tpname);
        _coverage.prepare(url, tp);
        if ( !_coverage.isValid())
            return sPREPAREFAILED;
        _coverage->loadData(); // loaddata now so that all attribute info( ranges) is known; we need to load the data anyway.

    }else {
        _drawerCode = _expression.input<QString>(1);
    }
    _rootDrawer =  (DrawerInterface *)  (*iter).second.value<void *>();


    return sPREPARED;
}

quint64 AddDrawer::createMetadata()
{
    OperationResource operation({"ilwis://operations/adddrawer"});
    operation.setSyntax("adddrawer(viewid, drawername[,datasource, typename])");
    operation.setDescription(TR("adds a new drawer to the layerview identified by viewid"));
    operation.setInParameterCount({2,4});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("Drawer name/code"),TR("The name of the drawer or the type of drawer will be selected based on this parameter"));
    operation.addInParameter(2,itSTRING , TR("Data source"),TR("The url that is used to retrieve the data for this layer"));
    operation.addInParameter(3,itSTRING , TR("Data source typename"),TR("which data type is represented by this url"));
    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

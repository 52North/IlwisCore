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
#include "../drawercolorsets.h"
#include "adddrawer.h"

#include "rootdrawer.h"

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
        LayerDrawer *ldrawer = DrawerFactory::create<LayerDrawer>(_coverage->ilwisType(), rootdrawer, rootdrawer, _options /* IOOptions() */);
        if ( !ldrawer){
            return ERROR2(ERR_NO_INITIALIZED_2,"Drawer",_coverage->name());
        }
        ldrawer->coverage(_coverage);
        if ( rootdrawer->drawerCount(Geodrawer::ComplexDrawer::dtMAIN) == 0)
            rootdrawer->coordinateSystem(_coverage->coordinateSystem());
        rootdrawer->addSpatialDrawer(ldrawer,false);
        QVariant var = qVariantFromValue((void *)ldrawer);
        ctx->addOutput(symTable,var,"layerdrawer",_coverage->ilwisType(), Resource());
    }else if ( _drawerCode != ""){
        drawer = Geodrawer::DrawerFactory::create<>(_drawerCode, rootdrawer, rootdrawer, _options /*IOOptions()*/ );
        rootdrawer->addDrawer(drawer,drawer->drawerType(),drawer->defaultOrder());
    }
    logOperation(_expression);
    rootdrawer->redraw();
    return true;
}

Ilwis::OperationImplementation *AddDrawer::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AddDrawer(metaid, expr);
}

Ilwis::OperationImplementation::State AddDrawer::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    auto iter = ctx->_additionalInfo.find("rootdrawer");
    if ( iter == ctx->_additionalInfo.end())
        return sPREPAREFAILED;

     _rootDrawer =  (DrawerInterface *)  (*iter).second.value<void *>();
    int parameterCount = _expression.parameterCount();
    if ( parameterCount == 4){
        QString url = _expression.input<QString>(1);
        QString filter = _expression.input<QString>(2);
        QString tpname = _expression.input<QString>(3);

        IlwisTypes tp = IlwisObject::name2Type(tpname);
        if ( !hasType(tp , itCOVERAGE)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("dataype for layer drawer"), tpname)    ;
            return sPREPAREFAILED;
        }
        if ( filter.indexOf("itemid=") != -1 || filter.indexOf("resource=") != -1){
            if ( filter[0] == '\"')
                filter = filter.mid(1, filter.size() - 2);
            std::vector<Resource> res = mastercatalog()->select(filter);
            if ( res.size() != 1){
                kernel()->issues()->log(QString("Could not open as %1, %2").arg(tpname).arg(url));
                return sPREPAREFAILED;
            }
            _coverage.prepare(res[0]);
        }else{
           _coverage.prepare(filter, tp);
        }        

        if ( !_coverage.isValid()){
            kernel()->issues()->log(QString("Could not open as %1, %2").arg(tpname).arg(url));
            return sPREPAREFAILED;
        }
         QVariant var = _rootDrawer->attribute("maindrawercount");
         if ( _coverage->coordinateSystem()->isUnknown() && var.toInt() > 0){
             QString mes = QString("coordinate system 'unknown' not compatible with coordinate system of the layerview");
             kernel()->issues()->log(mes, IssueObject::itWarning);
             return sPREPAREFAILED;
         }
        //_coverage->loadData();
    } else {
        if (parameterCount == 3) {
            QString colorset = _expression.input<QString>(2);
            if (colorset.size() > 0) {
                DrawerColorsets cs;
                _options.addOption("areacolor", cs[colorset].area);
                _options.addOption("bordercolor",cs[colorset].border);
            }
        }
        _drawerCode = _expression.input<QString>(1);
    }

    return sPREPARED;
}

quint64 AddDrawer::createMetadata()
{
    OperationResource operation({"ilwis://operations/adddrawer"});
    operation.setSyntax("adddrawer(viewid, drawername[,!datasource,!typename][,!areacolor,!drawtarget])");
    operation.setDescription(TR("adds a new drawer to the layerview identified by viewid"));
    operation.setInParameterCount({2,3,4,6});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("Drawer name/code"),TR("The name of the drawer or the type of drawer will be selected based on this parameter"));
    operation.addOptionalInParameter(2,itSTRING , TR("Data source"),TR("The url that is used to retrieve the data for this layer"));
    operation.addOptionalInParameter(3,itSTRING , TR("Data source typename"),TR("which data type is represented by this url"));

    operation.addOptionalInParameter(4,itSTRING , TR("Area Color"),TR("Area colorset name for this selection drawer"));

    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

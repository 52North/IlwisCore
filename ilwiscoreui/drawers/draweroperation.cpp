#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "models/uicontextmodel.h"
#include "drawerinterface.h"
#include "draweroperation.h"

using namespace Ilwis;
using namespace Geodrawer;

DrawerOperation::DrawerOperation()
{

}

DrawerOperation::DrawerOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

DrawerOperation::~DrawerOperation()
{

}
DrawerInterface *DrawerOperation::getRootDrawer(const quint64 index){
    bool ok;
    quint64 viewerId = _expression.input<QString>(index).toULongLong(&ok);
    if (!ok){
        ERROR1(TR("Invalid viewer id %1"), _expression.input<QString>(index));
        return 0;
    }
    auto *viewer = uicontext()->viewer(viewerId);
    if (!viewer){
        ERROR1(TR("Invalid viewer id %1"), _expression.input<QString>(index));
        return 0;
    }
    return viewer->rootDrawer();
}

DrawerInterface *DrawerOperation::getRootDrawer(){

    return getRootDrawer(0);
}


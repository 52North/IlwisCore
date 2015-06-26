#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "uicontextmodel.h"
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
DrawerInterface *DrawerOperation::getRootDrawer(){
    bool ok;
    quint64 viewerId = _expression.input<QString>(0).toULongLong(&ok);
    if (!ok){
        ERROR1(TR("Invalid viewer id %1"), _expression.input<QString>(0));
        return 0;
    }
    auto *viewer = uicontext()->viewer(viewerId);
    if (!viewer){
        ERROR1(TR("Invalid viewer id %1"), _expression.input<QString>(0));
        return 0;
    }
    return viewer->rootDrawer();
}


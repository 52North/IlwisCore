#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
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


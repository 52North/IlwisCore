#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "nodepropobject.h"

using namespace Ilwis;

NodePropObject::NodePropObject(QObject *p) : QObject(p)
{
}

void NodePropObject::setProps(const NodeProperties& props, int v) {
    _x = props._x;
    _y = props._y;
    _vertex = v;
    _operationId = props._operationid;
}

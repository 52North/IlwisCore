#include "kernel.h"
#include "ilwisdata.h"
#include "mastercatalog.h"
#include "workflow.h"
#include "edgepropobject.h"

using namespace Ilwis;

EdgePropObject::EdgePropObject(QObject *p) : QObject(p)
{
}

void EdgePropObject::setProps(const EdgeProperties& props, int v1, int v2) {
    _outputParameterIndex = props._outputParameterIndex;
    _inputParameterIndex = props._inputParameterIndex;
    _outputRectangleIndex = props._outputRectangleIndex;
    _inputRectangleIndex = props._inputRectangleIndex;
    _outputVertexIndex = v1;
    _intputVertexIndex = v2;
}

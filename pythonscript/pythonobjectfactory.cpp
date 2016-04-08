#include "kernel.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "pythonobjectfactory.h"

using namespace Ilwis;
using namespace PythonScript;

PythonObjectFactory::PythonObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","python","")
{

}

IlwisObject *Ilwis::PythonScript::PythonObjectFactory::create(const Ilwis::Resource &resource, const IOOptions &options) const
{

}

bool PythonObjectFactory::canUse(const Resource &resource) const
{
    return false;
}

bool PythonObjectFactory::prepare()
{
    return true;
}

#include <QUrl>
#include "kernel.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"

using namespace Ilwis;

IlwisObjectConnector::IlwisObjectConnector(const Ilwis::Resource &resource, bool) : _resource(resource)
{
}

IlwisTypes IlwisObjectConnector::type() const
{
    return _resource.ilwisType();
}

Resource &IlwisObjectConnector::source()
{
    return _resource;
}

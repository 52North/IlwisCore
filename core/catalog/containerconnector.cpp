#include "kernel.h"
#include "connectorinterface.h"
#include "containerconnector.h"

using namespace Ilwis;

ContainerConnector::ContainerConnector()
{
}

ContainerConnector::ContainerConnector(const Resource &resource) : _container(resource)
{
    if (resource.ilwisType() != itCONTAINER)
        _container = Resource();
}

Resource& ContainerConnector::source()
{
    return _container;
}

Resource ContainerConnector::source() const
{
    return _container;
}

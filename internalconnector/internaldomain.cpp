#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internaldomain.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalDomainConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalDomainConnector(resource, load, options);
}

InternalDomainConnector::InternalDomainConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalDomainConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *InternalDomainConnector::create() const
{
    return 0;
}

bool InternalDomainConnector::loadData(IlwisObject *, const IOOptions &)
{
    return true;
}

QString InternalDomainConnector::provider() const
{
    return "internal";
}

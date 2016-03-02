#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalprojectionconnector.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalProjectionConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalProjectionConnector(resource, load, options);
}

InternalProjectionConnector::InternalProjectionConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalProjectionConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *InternalProjectionConnector::create() const
{
    return 0;
}

bool InternalProjectionConnector::loadData(IlwisObject *, const IOOptions &)
{
    return true;
}

QString InternalProjectionConnector::provider() const
{
    return "internal";
}

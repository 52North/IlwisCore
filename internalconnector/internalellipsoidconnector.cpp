#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalellipsoidconnector.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalEllipsoidConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalEllipsoidConnector(resource, load, options);
}

InternalEllipsoidConnector::InternalEllipsoidConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalEllipsoidConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *InternalEllipsoidConnector::create() const
{
    return 0;
}

bool InternalEllipsoidConnector::loadData(IlwisObject *, const IOOptions &)
{
    return true;
}

QString InternalEllipsoidConnector::provider() const
{
    return "internal";
}



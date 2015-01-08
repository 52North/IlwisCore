#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "colorlookup.h"
#include "representation.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalrepresentation.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalRepresentationConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    return new InternalRepresentationConnector(resource, load, options);
}

InternalRepresentationConnector::InternalRepresentationConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalRepresentationConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *InternalRepresentationConnector::create() const
{
    return 0;
}

bool InternalRepresentationConnector::loadData(IlwisObject *, const IOOptions &)
{
    return true;
}

QString InternalRepresentationConnector::provider() const
{
    return "internal";
}

#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalcoordinatesystemconnector.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalCoordinatesystemConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    Resource res = resource;
    if ( resource.code() == "unknown"){
        quint64 id = IlwisObject::internalname2id(resource.name(), false);
        if ( id != i64UNDEF)
            res.setId(id); // every unknown has its own id; is already in the resource name.
    }
    return new InternalCoordinatesystemConnector(resource, load, options);
}

InternalCoordinatesystemConnector::InternalCoordinatesystemConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalCoordinatesystemConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *InternalCoordinatesystemConnector::create() const
{
    return 0;
}

bool InternalCoordinatesystemConnector::loadData(IlwisObject *, const IOOptions& )
{
    return true;
}

QString InternalCoordinatesystemConnector::provider() const
{
    return "internal";
}


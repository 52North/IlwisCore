#include "kernel.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "operationmetadataconnector.h"


using namespace Ilwis;
using namespace Internal;

ConnectorInterface *OperationMetadataConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    Resource res = resource;
    if ( resource.code() == "unknown"){
        quint64 id = IlwisObject::internalname2id(resource.name(), false);
        if ( id != i64UNDEF)
            res.setId(id); // every unknown has its own id; is already in the resource name.
    }
    return new OperationMetadataConnector(resource, load, options);
}

OperationMetadataConnector::OperationMetadataConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool OperationMetadataConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

IlwisObject *OperationMetadataConnector::create() const
{
    return 0;
}

bool OperationMetadataConnector::loadData(IlwisObject *, const IOOptions& )
{
    return true;
}

QString OperationMetadataConnector::provider() const
{
    return "internal";
}


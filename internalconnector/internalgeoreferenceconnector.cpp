#include "kernel.h"
#include "ilwisdata.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalgeoreferenceconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalGeoReferenceConnector::create(const Ilwis::Resource &resource,bool load,const IOOptions& options)
{
    Resource res = resource;
    if ( resource.code() == "undetermined"){
        quint64 id = IlwisObject::internalname2id(resource.name(), false);
        if ( id != i64UNDEF)
            res.setId(id); // every undetermined has its own id; is already in the resource name.
    }
    return new InternalGeoReferenceConnector(res, load, options);
}

InternalGeoReferenceConnector::InternalGeoReferenceConnector(const Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalGeoReferenceConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}


IlwisObject *InternalGeoReferenceConnector::create() const
{
    return 0;
}

bool InternalGeoReferenceConnector::loadData(IlwisObject *, const IOOptions &)
{
    return true;
}

QString InternalGeoReferenceConnector::provider() const
{
    return "internal";
}

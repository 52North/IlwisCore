#include "kernel.h"
#include "ilwisdata.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "box.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "internalgeoreferenceconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalGeoReferenceConnector::create(const Ilwis::Resource &resource,bool load,const PrepareOptions& options)
{
    return new InternalGeoReferenceConnector(resource, load, options);
}

InternalGeoReferenceConnector::InternalGeoReferenceConnector(const Resource &resource, bool load, const PrepareOptions &options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalGeoReferenceConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options)
{
    return true;
}


IlwisObject *InternalGeoReferenceConnector::create() const
{
    return 0;
}

bool InternalGeoReferenceConnector::loadData(IlwisObject *obj)
{
    return true;
}

QString InternalGeoReferenceConnector::provider() const
{
    return "internal";
}

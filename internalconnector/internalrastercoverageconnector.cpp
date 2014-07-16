#include "kernel.h"
#include "raster.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalrastercoverageconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalRasterCoverageConnector::create(const Ilwis::Resource &resource,bool load,const PrepareOptions& options)
{
    return new InternalRasterCoverageConnector(resource, load, options);
}

InternalRasterCoverageConnector::InternalRasterCoverageConnector(const Resource &resource, bool load,const PrepareOptions& options) : IlwisObjectConnector(resource, load, options)
{
}

bool InternalRasterCoverageConnector::loadMetaData(IlwisObject *data, const PrepareOptions &options){
    RasterCoverage *gcoverage = static_cast<RasterCoverage *>(data);
    if(_dataType == gcoverage->datadef().range().isNull())
        return false;
    if ( !gcoverage->datadef().range().isNull())
        _dataType = gcoverage->datadef().range()->determineType();

    gcoverage->gridRef()->prepare(0,gcoverage->size());

    return true;
}

bool InternalRasterCoverageConnector::loadData(IlwisObject* data, const LoadOptions &options){
    return true;
}

QString Ilwis::Internal::InternalRasterCoverageConnector::provider() const
{
    return "internal";
}

IlwisObject *InternalRasterCoverageConnector::create() const
{
    return new RasterCoverage();
}






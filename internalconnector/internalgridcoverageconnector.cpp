#include "kernel.h"
#include "raster.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "columndefinition.h"
#include "table.h"
#include "ilwisobjectconnector.h"
#include "InternalGridCoverageConnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalGridCoverageConnector::create(const Ilwis::Resource &item,bool load)
{
    return new InternalGridCoverageConnector(item, load);
}

InternalGridCoverageConnector::InternalGridCoverageConnector(const Resource &item, bool load) : IlwisObjectConnector(item, load)
{
}

bool InternalGridCoverageConnector::loadMetaData(IlwisObject *data){
    RasterCoverage *gcoverage = static_cast<RasterCoverage *>(data);
    if(_dataType == gcoverage->datadef().range().isNull())
        return false;
    if ( !gcoverage->datadef().range().isNull())
        _dataType = gcoverage->datadef().range()->determineType();

    return true;
}

Grid *InternalGridCoverageConnector::loadGridData(IlwisObject* data){
    RasterCoverage *gc = static_cast<RasterCoverage *>(data);
    Grid *grid = new Grid(gc->size());
    grid->prepare();

    return grid;
}

QString Ilwis::Internal::InternalGridCoverageConnector::provider() const
{
    return "internal";
}

IlwisObject *InternalGridCoverageConnector::create() const
{
    return new RasterCoverage();
}






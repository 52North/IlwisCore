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
    GridCoverage *gcoverage = static_cast<GridCoverage *>(data);
    if(_dataType == gcoverage->datadef().range().isNull())
        return false;
    if ( !gcoverage->datadef().range().isNull())
        _dataType = gcoverage->datadef().range()->determineType();

    return true;
}

Grid *InternalGridCoverageConnector::loadGridData(IlwisObject* data){
    GridCoverage *gc = static_cast<GridCoverage *>(data);
    Grid *grid = new Grid(gc->size());
    grid->prepare();

    return grid;
}

IlwisObject *InternalGridCoverageConnector::create() const
{
    return new GridCoverage();
}






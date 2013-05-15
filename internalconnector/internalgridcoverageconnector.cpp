#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "angle.h"
#include "point.h"
#include "box.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "coordinatesystem.h"
#include "valuedefiner.h"
#include "columndefinition.h"
#include "table.h"
#include "containerstatistics.h"
#include "coverage.h"
#include "containerstatistics.h"
#include "georeference.h"
#include "grid.h"
#include "gridcoverage.h"
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
    if(_dataType == gcoverage->range().isNull())
        return false;
    if ( !gcoverage->range().isNull())
        _dataType = gcoverage->range()->determineType();

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






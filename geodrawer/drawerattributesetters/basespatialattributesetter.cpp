#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseSpatialAttributeSetter::BaseSpatialAttributeSetter(const IOOptions &)
{

}

BaseSpatialAttributeSetter::~BaseSpatialAttributeSetter()
{

}

void BaseSpatialAttributeSetter::sourceCsySystem(const ICoordinateSystem &source)
{
    _sourceSystem = source;
}

void BaseSpatialAttributeSetter::targetCsySystem(const ICoordinateSystem &target)
{
    _targetSystem = target;
}

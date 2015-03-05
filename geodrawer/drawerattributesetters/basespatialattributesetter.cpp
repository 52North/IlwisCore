#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "drawers/drawerinterface.h"
#include "basespatialattributesetter.h"

using namespace Ilwis;
using namespace Geodrawer;

BaseSpatialAttributeSetter::BaseSpatialAttributeSetter(const IOOptions &options)
{
    if ( options.contains("rootdrawer")){
        _rootDrawer = (DrawerInterface *)  options["rootdrawer"].value<void *>();
        _targetSystem = _rootDrawer->attribute("coordinatesystem").value<ICoordinateSystem>();
    }
}

BaseSpatialAttributeSetter::~BaseSpatialAttributeSetter()
{

}

void BaseSpatialAttributeSetter::sourceCsySystem(const ICoordinateSystem &source)
{
    _sourceSystem = source;
    if ( _targetSystem.isValid() && _sourceSystem.isValid()){
        _conversionNeeded = _sourceSystem != _targetSystem;
    }
}

bool BaseSpatialAttributeSetter::isValid() const
{
    return _rootDrawer != 0 && _targetSystem.isValid() && _sourceSystem.isValid();
}

bool BaseSpatialAttributeSetter::coordinateConversionNeeded() const
{
    return _conversionNeeded;
}

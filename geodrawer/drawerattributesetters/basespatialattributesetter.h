#ifndef BASESPATIALATTRIBUTESETTER_H
#define BASESPATIALATTRIBUTESETTER_H

#include "drawers/drawerattributesetter.h"

namespace Ilwis {
namespace Geodrawer{


class BaseSpatialAttributeSetter : public DrawerAttributeSetter
{
public:
    BaseSpatialAttributeSetter(const Ilwis::IOOptions &options);
    ~BaseSpatialAttributeSetter();

    bool isValid() const;
    bool coordinateConversionNeeded() const;

    void sourceCsySystem(const ICoordinateSystem &source);
protected:
    ICoordinateSystem _sourceSystem;
    ICoordinateSystem _targetSystem;
    DrawerInterface *_rootDrawer = 0;
    bool _conversionNeeded = false;
    Envelope _zoomRect;
};
}
}

#endif // BASESPATIALATTRIBUTESETTER_H

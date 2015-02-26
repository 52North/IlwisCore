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

    void sourceCsySystem(const ICoordinateSystem& source);
    void targetCsySystem(const ICoordinateSystem& target);

protected:
    ICoordinateSystem _sourceSystem;
    ICoordinateSystem _targetSystem;
};
}
}

#endif // BASESPATIALATTRIBUTESETTER_H

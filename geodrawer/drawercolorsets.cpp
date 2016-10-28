#include "drawercolorsets.h"


using namespace Ilwis;
using namespace Geodrawer;


DrawerColorsets::DrawerColorsets()
{
    QColor area(80,80,0);
    area.setAlphaF(0.15);

    QColor border(0,0,0);
    border.setAlphaF(1);
    _colorsets.insert("basic",Colorset { area, border } );

    QColor area2(0,0,0);
    area2.setAlphaF(0.0);
    QColor border2(255,255,0);
    border2.setAlphaF(0);
    _colorsets.insert("colorless",Colorset { area2, border2 } );
}


Colorset DrawerColorsets::GetColorset(const QString &colorset) const {

    return _colorsets[colorset];
}

Colorset DrawerColorsets::operator [](const QString &key) const
{
    return GetColorset(key);
}

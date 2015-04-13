#ifndef RASTERVALUEIMAGE_H
#define RASTERVALUEIMAGE_H

#include "drawers/rasterimage.h"

namespace Ilwis{
namespace Geodrawer {

class RasterValueImage : public RasterImage
{
public:
    RasterValueImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options=IOOptions());
    ~RasterValueImage();

    bool prepare(int prepareType);

private:
    QVector<QRgb> _colorTable;
    std::vector<quint8> _pixels;

    static RasterImage *create(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);

NEW_RASTERIMAGETYPE
};
}
}

#endif // RASTERVALUEIMAGE_H

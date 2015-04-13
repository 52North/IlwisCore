#ifndef RASTERCOLORIMAGE_H
#define RASTERCOLORIMAGE_H

#include "drawers/rasterimage.h"


namespace Ilwis{
namespace Geodrawer {
class RasterColorImage : public RasterImage
{
public:
    RasterColorImage(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute,const IOOptions &options);
    ~RasterColorImage();
    bool prepare(int prepareType);

private:
    static     RasterImage *create(DrawerInterface *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute, const IOOptions &options);

    std::vector<VertexColorI> _pixels;

    NEW_RASTERIMAGETYPE
};
}
}

#endif // RASTERCOLORIMAGE_H

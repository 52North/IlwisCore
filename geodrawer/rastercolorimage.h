#ifndef RASTERCOLORIMAGE_H
#define RASTERCOLORIMAGE_H

#include "rasterimage.h"


namespace Ilwis{
namespace Geodrawer {
class RasterColorImage : public RasterImage
{
public:
    RasterColorImage(RootDrawer *rootDrawer, const IRasterCoverage &raster, const VisualAttribute &vattribute);
    ~RasterColorImage();
    bool prepare(int prepareType);

private:
    std::vector<VertexColorI> _pixels;
};
}
}

#endif // RASTERCOLORIMAGE_H

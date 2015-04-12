#ifndef RASTERVALUEIMAGE_H
#define RASTERVALUEIMAGE_H

#include "rasterimage.h"

namespace Ilwis{
namespace Geodrawer {

class RasterValueImage : public RasterImage
{
public:
    RasterValueImage(Ilwis::Geodrawer::RootDrawer *rootDrawer, const Ilwis::IRasterCoverage &raster, const Ilwis::Geodrawer::VisualAttribute &vattribute);
    ~RasterValueImage();

    bool prepare(int prepareType);
private:
    QVector<QRgb> _colorTable;
    std::vector<quint8> _pixels;
};
}
}

#endif // RASTERVALUEIMAGE_H

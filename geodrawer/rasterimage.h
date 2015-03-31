#ifndef RASTERIMAGE_H
#define RASTERIMAGE_H

namespace Ilwis{
namespace Geodrawer {

class RasterImage : public QImage
{
public:
    RasterImage();
    RasterImage(const IRasterCoverage& raster);
    ~RasterImage();

    void setSelection(const BoundingBox& box);

private:
    IRasterCoverage _raster;


};
}
}

#endif // RASTERIMAGE_H

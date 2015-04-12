#ifndef RASTERIMAGE_H
#define RASTERIMAGE_H
class QImage;

namespace Ilwis{
namespace Geodrawer {
class RootDrawer;

class RasterImage
{
public:
    RasterImage(RootDrawer *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute);
    virtual ~RasterImage();

    const std::unique_ptr<QImage>& image();
    virtual bool prepare(int prepareType) = 0;
    void visualAttribute(const VisualAttribute &vattribute);
protected:
    IRasterCoverage _raster;
    RootDrawer *_rootDrawer;
    std::unique_ptr<QImage> _image;
    VisualAttribute _visualAttribute;




};
}
}

#endif // RASTERIMAGE_H

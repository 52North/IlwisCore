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
    ~RasterImage();

    const std::unique_ptr<QImage>& image();
    void makeImage();
private:
    IRasterCoverage _raster;
    RootDrawer *_rootDrawer;
    VisualAttribute _visualAttribute;
    std::unique_ptr<QImage> _image;
    std::vector<VertexColorI> _pixels;


};
}
}

#endif // RASTERIMAGE_H

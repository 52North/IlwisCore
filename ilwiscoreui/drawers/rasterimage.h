#ifndef RASTERIMAGE_H
#define RASTERIMAGE_H

#include "ilwiscoreui_global.h"

class QImage;

namespace Ilwis{
namespace Geodrawer {
class RootDrawer;
class DrawerInterface;

class ILWISCOREUISHARED_EXPORT RasterImage
{
public:
    RasterImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options=IOOptions());
    virtual ~RasterImage();

    const std::unique_ptr<QImage>& image();
    virtual bool prepare(int prepareType) = 0;
    void visualAttribute(const VisualAttribute &vattribute);
protected:
    IRasterCoverage _raster;
    DrawerInterface *_rootDrawer;
    std::unique_ptr<QImage> _image;
    VisualAttribute _visualAttribute;




private:
    void makeImageTemp();
};
#define NEW_RASTERIMAGETYPE \
    private:\
        static RasterImage *dummy_raster_image;

#define REGISTER_RASTERIMAGETYPE(classname,tp) \
    RasterImage *classname::dummy_raster_image = RasterImageFactory::registerRasterImageType(#classname,tp,classname::create);
}
}

#endif // RASTERIMAGE_H

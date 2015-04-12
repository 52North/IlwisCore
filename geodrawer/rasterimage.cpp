#include <QImage>
#include "raster.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "drawers/attributevisualproperties.h"
#include "rasterimage.h"

using namespace Ilwis;
using namespace Geodrawer;

RasterImage::RasterImage(RootDrawer *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute) :
    _raster(raster), _rootDrawer(rootDrawer), _visualAttribute(vattribute)
{

}

RasterImage::~RasterImage()
{
}

const std::unique_ptr<QImage> &RasterImage::image()
{
    return _image;
}

void RasterImage::visualAttribute(const VisualAttribute &vattribute)
{
    _visualAttribute = vattribute;
}









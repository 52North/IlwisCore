#include <QImage>
#include "raster.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "drawers/rasterimagefactory.h"
#include "drawers/attributevisualproperties.h"

#include "rastervalueimage.h"

using namespace Ilwis;
using namespace Geodrawer;

REGISTER_RASTERIMAGETYPE(RasterValueImage,itNUMERICDOMAIN);

RasterValueImage::RasterValueImage(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options) :
    RasterImage(rootDrawer,raster,vattribute, options)
{
}

RasterValueImage::~RasterValueImage()
{

}

RasterImage *RasterValueImage::create(DrawerInterface *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute,const IOOptions &options)
{
    return new RasterValueImage(rootDrawer, raster, vattribute, options)    ;
}

bool RasterValueImage::prepare(int prepareType)
{
    bool resetImage = false;
    BoundingBox bb(_raster->size());
    int xl = (quint32)bb.xlength() % 4 == 0 ? bb.xlength() : (((quint32)bb.xlength() / 4) + 1) * 4;
    int rest = xl - bb.xlength();
    if ( hasType(prepareType, DrawerInterface::ptRENDER)){
        std::vector<QColor> colors = _visualAttribute.colors();
        _colorTable = QVector<QRgb>(colors.size()) ;
        for(int i = 0; i < _colorTable.size(); ++i)
            _colorTable[i] = colors[i].rgba();
        resetImage = true;

    }

    if ( hasType(prepareType, DrawerInterface::ptGEOMETRY)){
        quint32 size = xl * bb.ylength();
        _pixels.resize(size);
        PixelIterator pixIter(_raster);




        SPNumericRange numrange =  _raster->datadef().range<NumericRange>();
        auto end = pixIter.end();
        quint32 position = 0;
        while(pixIter != end){
            double value = *pixIter;
            int index = value == rUNDEF ? 0 : 1 + (_colorTable.size() - 1) * (value - numrange->min()) / numrange->distance();
            _pixels[position] = index;
            ++pixIter;
            if ( pixIter.ychanged()){
                position += rest;
            }
            ++position;
        }
        resetImage = true;
    }

    if ( resetImage)   {
        const uchar *datablock = (const uchar *)_pixels.data();
        _image.reset(new QImage(datablock,xl, bb.ylength(),QImage::Format_Indexed8));
        _image->setColorTable(_colorTable);
    }
    return resetImage;

}



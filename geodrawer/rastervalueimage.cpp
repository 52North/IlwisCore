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
    // lines must be 32 bit alligned. an entry pixel is a byte, so everything must be 4 alligned (32 bit is 4 bytes)
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

void RasterImage::makeImageTemp()
{
    // doesnt work, but i keep it around for later checking, should be more efficient version of makeImage()
//    BoundingBox bb(_raster->size()); // = _rootDrawer->screenGrf()->coord2Pixel(_raster->envelope());
//    Size<> pixArea = _rootDrawer->pixelAreaSize();
//    double dx = std::max(1.0,(double)bb.xlength() / pixArea.xsize());
//    double dy = std::max(1.0,(double)bb.ylength() / pixArea.ysize());
//    std::vector<double> xsteps(pixArea.xsize(),0);
//    std::vector<double> ysteps(pixArea.ysize(),0);

//    for(int x = 1; x < xsteps.size(); ++x){
//        xsteps[x] = xsteps[x-1] + dx;
//    }
//    for(int x = 1; x < xsteps.size(); ++x){
//        xsteps[x] = (int)(xsteps[x] - (int)((x - 1) * dx));

//    }
//    for(int y = 1; y < ysteps.size(); ++y){
//        ysteps[y] = ysteps[y-1] + dy;
//    }
//    for(int y = 1; y < ysteps.size(); ++y){
//        ysteps[y] = (int)(ysteps[y] - (int)((y - 1) * dy));
//    }

//     _pixels.resize(pixArea.linearSize());
//    PixelIterator pixIter(_raster);
//    int xindex = 1;
//    int yindex = 1;

//    while(yindex != ysteps.size()){
//        double value = *pixIter;
//        auto color = _visualAttribute.value2color(value);
//        _pixels[yindex * pixArea.xsize() + xindex] = color;
//        pixIter += xsteps[xindex++];
//        if ( pixIter.ychanged()){
//            pixIter = Pixel(0,pixIter.y() + ysteps[yindex++]);
//            xindex = 0;
//        }

//    }

//    const uchar *datablock = (const uchar *)_pixels.data();
//    _image.reset(new QImage(datablock,pixArea.xsize(), pixArea.ysize(),QImage::Format_RGBA8888));
}



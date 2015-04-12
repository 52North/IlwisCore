#include <QImage>
#include "raster.h"
#include "pixeliterator.h"
#include "rootdrawer.h"
#include "drawers/attributevisualproperties.h"
#include "rastervalueimage.h"

using namespace Ilwis;
using namespace Geodrawer;

RasterValueImage::RasterValueImage(RootDrawer *rootDrawer, const IRasterCoverage& raster, const VisualAttribute &vattribute) :
    RasterImage(rootDrawer,raster,vattribute)
{
}

RasterValueImage::~RasterValueImage()
{

}

bool RasterValueImage::prepare(int prepareType)
{
    bool resetImage = false;
    BoundingBox bb(_raster->size());
    if ( hasType(prepareType, DrawerInterface::ptRENDER)){
        std::vector<QColor> colors = _visualAttribute.colors();
        _colorTable = QVector<QRgb>(colors.size()) ;
        for(int i = 0; i < _colorTable.size(); ++i)
            _colorTable[i] = colors[i].rgba();
        resetImage = true;

    }

    if ( hasType(prepareType, DrawerInterface::ptGEOMETRY)){
        BoundingBox bb(_raster->size());
        quint32 size = bb.xlength() * bb.ylength();
        _pixels.resize(size);
        PixelIterator pixIter(_raster);




        SPNumericRange numrange =  _raster->datadef().range<NumericRange>();
        auto end = pixIter.end();
        while(pixIter != end){
            double value = *pixIter;
            int index = value == rUNDEF ? 0 : 1 + (_colorTable.size() - 1) * (value - numrange->min()) / numrange->distance();
            _pixels[pixIter.linearPosition()] = index;
            ++pixIter;
        }
        resetImage = true;
    }

    if ( resetImage)   {
        const uchar *datablock = (const uchar *)_pixels.data();
        _image.reset(new QImage(datablock,bb.xlength(), bb.ylength(),QImage::Format_Indexed8));
        _image->setColorTable(_colorTable);
    }
    return resetImage;

}



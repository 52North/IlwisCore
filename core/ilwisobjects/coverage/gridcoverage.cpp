#include "kernel.h"
#include "raster.h"
#include "gridcoverage.h"
#include "pixeliterator.h"



quint64 Ilwis::GridBlockInternal::_blockid = 0;

using namespace Ilwis;

GridCoverage::GridCoverage()
{
}

GridCoverage::GridCoverage(const Resource& res) : Coverage(res){

}

GridCoverage::~GridCoverage()
{
}

const IGeoReference& GridCoverage::georeference() const
{
    return _georef;
}

void GridCoverage::georeference(const IGeoReference &grf)
{
    _georef = grf;
    if ( _grid.isNull() == false) { // remove the current grid, all has become uncertain
        _grid.reset(0);

    }
    if ( _georef.isValid()) {
        _georef->compute();
        setCoordinateSystem(grf->coordinateSystem()); // mandatory
    }
}

IlwisTypes GridCoverage::ilwisType() const
{
    return itGRID;
}


void GridCoverage::copyBinary(const IGridCoverage& gc, int index) {
    IGridCoverage gcNew;
    gcNew.set(this);
    Size inputSize =  gc->size();
    Size sz(inputSize.xsize(),inputSize.ysize(), 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(gc, Box3D<>(Voxel(0,0,index), Voxel(inputSize.xsize(), inputSize.ysize(), index + 1)));
    PixelIterator iterOut(gcNew, Box3D<>(Size(inputSize.xsize(), inputSize.ysize(), 1)));
    for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

Grid *GridCoverage::grid()
{
    Locker lock(_mutex);
    if ( _georef->isValid() && !connector().isNull()) {
        if ( _grid.isNull()) {
            _grid.reset(connector()->loadGridData(this));
            if (_grid.isNull())
                return 0;
        }
        Grid *grd = _grid.data();
        return grd;
    }
    return 0;
}

//bool GridCoverage::storeBinaryData() {
//    if (!connector(cmOUTPUT).isNull())
//        return connector(cmOUTPUT)->storeBinaryData(this);
//    return ERROR1(ERR_NO_INITIALIZED_1,"connector");
//}

Size GridCoverage::size() const
{
    if (!_grid.isNull())
        return _grid->size();
    if ( _georef.isValid())
        return _georef->size();

    return Size();

}

void GridCoverage::size(const Size &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        if (!_grid.isNull())
            _grid->setSize(sz);
        if (_georef.isValid())
            _georef->size(sz);
    }


}






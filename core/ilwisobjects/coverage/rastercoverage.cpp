#include "kernel.h"
#include "raster.h"
#include "rastercoverage.h"
#include "pixeliterator.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "numericitem.h"
#include "numericitemrange.h"
#include "ilwisoperation.h"

quint64 Ilwis::GridBlockInternal::_blockid = 0;

using namespace Ilwis;

RasterCoverage::RasterCoverage()
{
}

RasterCoverage::RasterCoverage(const Resource& resource) : Coverage(resource){

}

RasterCoverage::~RasterCoverage()
{
}

const IGeoReference& RasterCoverage::georeference() const
{
    return _georef;
}

void RasterCoverage::georeference(const IGeoReference &grf)
{
    _georef = grf;
    if ( _grid.isNull() == false) { // remove the current grid, all has become uncertain
        _grid.reset(0);

    }
    if ( _georef.isValid()) {
        _georef->compute();
        setCoordinateSystem(grf->coordinateSystem()); // mandatory
    }
    if (_georef.isValid())
        _size = _georef->size();
    else
        _size = Size();
}

IlwisTypes RasterCoverage::ilwisType() const
{
    return itRASTER;
}

RasterCoverage *RasterCoverage::copy()
{
    RasterCoverage *raster = new RasterCoverage();
    copyTo(raster);
    return raster;

}

const DataDefinition &RasterCoverage::datadef() const
{
    return _datadefCoverage;
}

DataDefinition &RasterCoverage::datadef()
{
    return _datadefCoverage;
}


void RasterCoverage::copyBinary(const IRasterCoverage& raster, int index) {
    IRasterCoverage gcNew;
    gcNew.set(this);
    Size inputSize =  raster->size();
    Size sz(inputSize.xsize(),inputSize.ysize(), 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(raster, Box3D<>(Voxel(0,0,index), Voxel(inputSize.xsize(), inputSize.ysize(), index + 1)));
    PixelIterator iterOut(gcNew, Box3D<>(Size(inputSize.xsize(), inputSize.ysize(), 1)));
    for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

//IRasterCoverage RasterCoverage::get(quint32 index1, quint32 index2)
//{
//    IRasterCoverage gcovParent;
//    gcovParent.set(this);
//    IRasterCoverage raster = OperationHelperRaster::initialize(gcovParent,itRASTER, itDOMAIN | itCOORDSYSTEM | itGEOREF);
//    raster->size(Size(size().xsize(), size().ysize()));
//    raster->_grid.reset(raster->_grid->clone(index1, index2));

//    return raster;

//}

Grid *RasterCoverage::grid()
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

void RasterCoverage::copyTo(IlwisObject *obj)
{
    Locker lock(_mutex);
    Coverage::copyTo(obj);
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    raster->_georef = _georef;
    raster->_datadefCoverage = _datadefCoverage;
    if ( _grid) {
        raster->_grid.reset(_grid->clone());
    }

}

Resource RasterCoverage::source(int mode) const
{
    Resource resource = Coverage::source(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        resource["georeference()"] = georeference()->id();
        resource.addProperty("domain", _datadefCoverage.domain()->id());
        resource.setExtendedType( resource.extendedType() | itGEOREF);
    }
    return resource;
}

Size RasterCoverage::size() const
{
    if (_size.isValid() && !_size.isNull())
        return _size;
    if (!_grid.isNull())
        const_cast<RasterCoverage *>(this)->_size = _grid->size();
    else if ( _georef.isValid())
        const_cast<RasterCoverage *>(this)->_size = _georef->size();

    return _size;

}

void RasterCoverage::unloadBinary() {
    if (_grid != 0) {
        return _grid->unload();
    }
}

void RasterCoverage::size(const Size &sz)
{
    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        _size = sz;
        if (!_grid.isNull())
            _grid->setSize(sz);
        if (_georef.isValid())
            _georef->size(sz);
    }
}








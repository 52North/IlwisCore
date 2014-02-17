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
    _georef.set(0);
}

const IGeoReference& RasterCoverage::georeference() const
{
    return _georef;
}

void RasterCoverage::georeference(const IGeoReference &grf)
{
    if ( isReadOnly())
        return;
    changed(true);

    _georef = grf;
    if ( _grid.isNull() == false) { // remove the current grid, all has become uncertain
        _grid.reset(0);

    }
    if ( _georef.isValid()) {
        _georef->compute();
        coordinateSystem(grf->coordinateSystem()); // mandatory
    }
    if (_georef.isValid())
        _size = _georef->size();
    else
        _size = Size<>();
}

IlwisTypes RasterCoverage::ilwisType() const
{
    return itRASTER;
}

RasterCoverage *RasterCoverage::clone()
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
    Size<> inputSize =  raster->size();
    Size<> sz(inputSize.xsize(),inputSize.ysize(), 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(raster, BoundingBox(Pixel(0,0,index), Pixel(inputSize.xsize(), inputSize.ysize(), index + 1)));
    PixelIterator iterOut(gcNew, BoundingBox(Size<>(inputSize.xsize(), inputSize.ysize(), 1)));
    for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

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
    raster->_size = _size;

}

Resource RasterCoverage::source(int mode) const
{
    Resource resource = Coverage::source(mode);
    if ( mode & IlwisObject::cmEXTENDED) {
        resource["georeference()"] = georeference()->id();
        resource.addProperty("domain", _datadefCoverage.domain()->id());
        resource.setExtendedType( resource.extendedType() | itDOMAIN |  itGEOREF);
    }
    return resource;
}

Size<> RasterCoverage::size() const
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

void RasterCoverage::size(const Size<> &sz)
{
    if ( isReadOnly())
        return;

    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        changed(true);
        _size = sz;
        if (!_grid.isNull())
            _grid->setSize(sz);
        if (_georef.isValid())
            _georef->size(sz);
    }
}








#include "kernel.h"
#include "raster.h"
#include "rastercoverage.h"
#include "pixeliterator.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "interval.h"
#include "intervalrange.h"
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

const DataDefinition &RasterCoverage::datadef(quint32 layer) const
{
    if ( layer == WHOLE_RASTER)
        return _datadefCoverage;
    if ( layer >= _datadefBands.size())
        throw ErrorObject(TR("invalid index for layer access"));

    return _datadefBands[layer];
}

DataDefinition &RasterCoverage::datadef(quint32 layer)
{
    if ( layer == WHOLE_RASTER)
        return _datadefCoverage;
    if ( layer >= _datadefBands.size())
        _datadefBands.resize(layer + 1);

    return _datadefBands[layer];
}


void RasterCoverage::copyBinary(const IRasterCoverage& raster, quint32 inputIndex, quint32 outputIndex) {
    if ( isNumericalUndef(inputIndex) || isNumericalUndef(outputIndex)){
        ERROR2(ERR_ILLEGAL_VALUE_2,TR("layer index"), isNumericalUndef(inputIndex) ? "input" : "output");
        return;
    }
    if ( inputIndex >= size().zsize()){
       ERROR2(ERR_ILLEGAL_VALUE_2,TR("layer index"), "input");
    }
    IRasterCoverage gcNew;
    gcNew.set(this);
    Size<> inputSize =  raster->size();
    Size<> sz(inputSize.xsize(),inputSize.ysize(), outputIndex + 1);
    gcNew->georeference()->size(sz);
    PixelIterator iterIn(raster, BoundingBox(Pixel(0,0,inputIndex), Pixel(inputSize.xsize(), inputSize.ysize(), inputIndex + 1)));
    PixelIterator iterOut(gcNew, BoundingBox(Pixel(0,0,outputIndex), Pixel(inputSize.xsize(), inputSize.ysize(), outputIndex + 1)));
    if ( raster->id() == id() && inputIndex == outputIndex){
        ERROR2(ERR_ILLEGALE_OPERATION2, TR("copy"),TR("identical layers in same raster"));
        return;
    }
    for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

NumericStatistics &RasterCoverage::statistics(int mode)
{
    if ( mode == ContainerStatistics<double>::pNONE)
        return Coverage::statistics(mode);
    IRasterCoverage raster(this);
    PixelIterator iter(raster);
    statistics().calculate(iter, iter.end(), (ContainerStatistics<double>::PropertySets)mode);

    return Coverage::statistics(mode);
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
        resource.addProperty("georeference", georeference()->id());
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

PixelIterator RasterCoverage::end()
{
   const IRasterCoverage raster(this);
   PixelIterator iter(raster);
   return iter.end();

}

PixelIterator RasterCoverage::begin()
{
    IRasterCoverage raster(this);
    return PixelIterator(raster);
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








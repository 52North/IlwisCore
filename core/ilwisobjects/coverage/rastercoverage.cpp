#include "raster.h"
#include "connectorinterface.h"
#include "symboltable.h"
#include "table.h"
#include "pixeliterator.h"

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

void RasterCoverage::georeference(const IGeoReference &grf, bool resetData)
{
    if ( isReadOnly())
        return;
    changed(true);

    _georef = grf;
    if ( resetData)
        _grid.reset(0);
    else if ( !_grid || grf->size().twod() != _grid->size().twod() ) {
        _grid.reset(0);

    }
    if ( _georef.isValid() ) {
        _georef->compute();
        coordinateSystem(grf->coordinateSystem()); // mandatory
    }
    if (_georef.isValid()){
        if ( _size.isValid() && !_size.isNull() && !resetData)
            _size = Size<>(_georef->size().xsize(), _georef->size().ysize(), _size.zsize());
        else
            _size = _georef->size();
    }
    else
        _size = Size<>();
    if (!_grid && _size.isValid()){
            gridRef()->prepare(this,_size);
    }
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

DataDefinition &RasterCoverage::datadefRef(quint32 layer)
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
       //ERROR2(ERR_ILLEGAL_VALUE_2,TR("layer index"), "input");
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
    std::for_each(iterOut, iterOut.end(), [&](double& v){
         v = *iterIn;
        ++iterIn;
    });
}

NumericStatistics &RasterCoverage::statistics(int mode, int bins)
{
    if ( mode == ContainerStatistics<double>::pNONE)
        return Coverage::statistics(mode);
    IRasterCoverage raster(this);
    PixelIterator iter(raster);
    statistics().calculate(iter, iter.end(), (ContainerStatistics<double>::PropertySets)mode, bins);
    auto rng = raster->datadefRef().range<NumericRange>();
    rng->min(statistics().prop(NumericStatistics::pMIN));
    rng->max(statistics().prop(NumericStatistics::pMAX));
    return Coverage::statistics(mode);
}

UPGrid &RasterCoverage::gridRef()
{
    if (!_grid)
        _grid.reset( new Grid);
    return _grid;
}

const UPGrid &RasterCoverage::grid() const
{
    if ( _grid)
        return _grid;
    throw ErrorObject(TR("Grid not yet initialized")) ;
}

void RasterCoverage::copyTo(IlwisObject *obj)
{
    Locker<> lock(_mutex);
    Coverage::copyTo(obj);
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    raster->_georef = _georef;
    raster->_datadefBands = _datadefBands;
    raster->_bandDefinition = _bandDefinition;
    raster->_datadefCoverage = _datadefCoverage;
    if ( _grid) {
        raster->_grid.reset(_grid->clone());
    }
    raster->_attributeTable = _attributeTable;
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
    if (_grid)
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

PixelIterator RasterCoverage::band(const QString &variantIndex)
{
    int index = _bandDefinition.index(variantIndex);
    if ( index >= size().zsize() || index < 0)
        return PixelIterator();

    return bandPrivate(index);
}

PixelIterator RasterCoverage::band(double bandIndex)
{
    int index = _bandDefinition.index(bandIndex);
    if ( index >= size().zsize() || index < 0)
        return PixelIterator();

    return bandPrivate(index);
}

PixelIterator RasterCoverage::bandPrivate(quint32 bandIndex)
{
    BoundingBox box(Pixel(0,0,bandIndex), Pixel(size().xsize()-1,size().ysize()-1, bandIndex));
    IRasterCoverage raster(this);
    return PixelIterator(raster,box);
}


bool RasterCoverage::band(const QString &bandIndex,  PixelIterator inputIter)
{
    if ( !_bandDefinition.domain()->contains(bandIndex))
        return false;
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex >= size().zsize()){
        _size.zsize(bndIndex + 1);
        _grid->setBandProperties(this, 1);
    }
    return bandPrivate(bndIndex, inputIter);
}

bool RasterCoverage::band(double bandIndex,  PixelIterator inputIter)
{
    if ( !_bandDefinition.domain()->contains(bandIndex))
        return false;
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex >= size().zsize()){
        _size.zsize(bndIndex + 1);
        _grid->setBandProperties(this, 1);
    }
    return bandPrivate(bndIndex, inputIter);
}

void RasterCoverage::setBandDefinition(QString bandIndex, const DataDefinition &def)
{
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex != iUNDEF){
        if ( bndIndex >= _datadefBands.size())
            _datadefBands.resize(bndIndex + 1);
        _datadefBands[bndIndex] = def;
    }
}

void RasterCoverage::setBandDefinition(double bandIndex, const DataDefinition &def)
{
    int bndIndex = _bandDefinition.index(bandIndex);
    if ( bndIndex != iUNDEF){
        if ( bndIndex >= _datadefBands.size())
            _datadefBands.resize(bndIndex + 1);
        _datadefBands[bndIndex] = def;
    }
}


bool RasterCoverage::bandPrivate(quint32 bandIndex,  PixelIterator inputIter)
{
    if ( inputIter.box().size().zsize() != 1)
        return false;
    bool isFirstLayer = !size().isValid() || size().isNull();
    if (!isFirstLayer)    { // if it is not the first layer, some rules for this raster have already been defined(2dsize + domain)
        if ( inputIter.box().xlength() != size().xsize() || inputIter.box().ylength() != size().ysize())
            return false;
        if (!inputIter.raster()->datadef().domain()->isCompatibleWith(datadef().domain().ptr()))
            return false;
    }


    if ( isFirstLayer ){ //  totally new band in new coverage, initialize everything

        coordinateSystem(inputIter.raster()->coordinateSystem());
        georeference(inputIter.raster()->georeference());
        datadefRef() = inputIter.raster()->datadef();
        envelope(inputIter.raster()->envelope());

        Size<> twodsz = inputIter.box().size().twod();
        size(Size<>(twodsz.xsize(), twodsz.ysize() ,stackDefinition().count()));
        _datadefBands.resize(stackDefinition().count());
    }
    datadefRef(bandIndex) = inputIter.raster()->datadef();
//    grid()->setBandProperties(this,bandIndex);

    PixelIterator iter = bandPrivate(bandIndex);
    while(iter != iter.end()){
        *iter = *inputIter;
        ++iter;
        ++inputIter;
    }
    return true;


}


void RasterCoverage::getData(quint32 blockIndex)
{
    if ( !connector().isNull()){
        connector()->loadData(this, {"blockindex", blockIndex});
    }
}

bool RasterCoverage::canUse(const IlwisObject *obj, bool strict) const
{
    if ( Coverage::canUse(obj, strict))
        return true;

    if ( hasType(obj->ilwisType(),itDOMAIN)){
        IDomain dom;
        dom.prepare(obj->id());
        if ( dom.isValid()){
            if (strict){

            }else {
                if ( hasType(dom->valueType(), itNUMBER) && hasType( datadef().domain()->valueType(), itNUMBER)){
                    return true;
                }
                if ( hasType(dom->ilwisType(), itITEMDOMAIN) && hasType( datadef().domain()->valueType(), itITEMDOMAIN)){
                    return datadef().domain()->isCompatibleWith(obj);
                }
            }
        }
    }
    return false;
}

void RasterCoverage::size(const Size<> &sz)
{
    if ( isReadOnly())
        return;

    // size must always be positive or undefined
    if (sz.xsize() > 0 && sz.ysize() > 0) {
        changed(true);
        _size = sz;
        gridRef()->prepare(this, sz);
        if (_georef.isValid())
            _georef->size(sz);
         stackDefinitionRef().setSubDefinition(sz.zsize()); // default filling, can be overruled
        for(int i = 0; i < sz.zsize(); ++i)
            datadefRef(i) = datadef();
    }
}

QVariant RasterCoverage::coord2value(const Coordinate &c, const QString &attrname)
{
    if ( _georef->isValid() && c.isValid()) {
        Pixeld pix = _georef->coord2Pixel(c);
        double value = pix2value(pix);
        if ( isNumericalUndef(value))
            return QVariant();
        if ( attrname != "")
            return value;
        else{
            QVariantMap vmap;
            if (!hasAttributes())
                vmap[PIXELVALUE] = value;
            else{
                for(int i=0; i < attributeTable()->columnCount(); ++i){
                    QVariant attrvalue = attributeTable()->cell(i, (quint32)value);
                    vmap[attributeTable()->columndefinitionRef(i).name()] = attrvalue;
                }
            }
            return QVariant(vmap);
        }
    }
    return QVariant();
}

ITable RasterCoverage::attributeTable() const
{
    return _attributeTable;
}

bool RasterCoverage::hasAttributes() const
{
    return _attributeTable.isValid();
}

void RasterCoverage::attributeTable(const ITable& tbl)
{
    if ( isReadOnly())
        return;
    changed(true);
    _attributeTable = tbl;
}

RasterStackDefinition &RasterCoverage::stackDefinitionRef()
{
    return _bandDefinition;
}

const RasterStackDefinition &RasterCoverage::stackDefinition() const
{
    return _bandDefinition;
}

void RasterCoverage::name(const QString &nam)
{
    if ( isReadOnly())
        return;
    changed(true);

    IlwisObject::name(nam);
    if ( _attributeTable.isValid()) {
        if ( _attributeTable->isAnonymous()) {
            _attributeTable->name(nam + "_attributes");
        }
    }
}

QString RasterCoverage::name() const
{
    return Identity::name();
}








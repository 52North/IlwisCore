#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <QColor>

#include "kernel.h"
#include "raster.h"
#include "georefimplementation.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"

#include "numericrange.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "pixeliterator.h"
#include "columndefinition.h"
#include "table.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "gdalproxy.h"
#include "ilwisobjectconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "coverageconnector.h"
#include "domainitem.h"
#include "itemrange.h"
#include "coloritem.h"
#include "colorrange.h"
#include "gridcoverageconnector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"

using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *RasterCoverageConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new RasterCoverageConnector(resource, load, options);

}

Ilwis::IlwisObject *RasterCoverageConnector::create() const{
    return new RasterCoverage(_resource);
}

RasterCoverageConnector::RasterCoverageConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : CoverageConnector(resource,load, options){
}


bool RasterCoverageConnector::loadMetaData(IlwisObject *data, const IOOptions &options){

    if(!CoverageConnector::loadMetaData(data, options))
        return false;

    auto *raster = static_cast<RasterCoverage *>(data);

    if (_handle->type() == GdalHandle::etGDALDatasetH){
        Coordinate cMin, cMax;
        quint32 layer = sourceRef().hasProperty("bandindex")? sourceRef()["bandindex"].toUInt(): iUNDEF;
        Size<> rastersize(gdal()->xsize(_handle->handle()), gdal()->ysize(_handle->handle()), layer != iUNDEF ? 1 : gdal()->layerCount(_handle->handle()));

        _offsetScales.resize(rastersize.zsize());

        std::vector<double> bands(layer != iUNDEF  ? 1 : rastersize.zsize());
        for(int i =0; i < rastersize.zsize(); ++i){
            bands[i] = i;
        }
        raster->stackDefinitionRef().setSubDefinition(IDomain("count"),bands);

        auto layerHandle = gdal()->getRasterBand(_handle->handle(), layer != iUNDEF  ? layer + 1 : 1);
        GDALColorInterp colorType = gdal()->colorInterpretation(layerHandle);
        bool ok = false;
        if ( layer != iUNDEF){
            raster->size(rastersize);

            if (sourceRef().hasProperty("scale") && sourceRef().hasProperty("offset")) {
                _offsetScales[layer].offset = sourceRef()["offset"].toDouble();
                _offsetScales[layer].scale = sourceRef()["scale"].toDouble();
            }

            ok = handleNumericLayerCase(layer, raster);
        }else if( colorType <=1 || layer != iUNDEF){ // no colors + grayscale which equals image domain
                    raster->size(rastersize);
                    ok = handleNumericCase(raster->size(), raster);
        } else if ( colorType >= 3 && colorType <= 12){
            raster->size(Size<>(rastersize.xsize(), rastersize.ysize(), layer != iUNDEF  ? 1 : rastersize.zsize() / 3));
            ok = handleColorCase(raster->size(), raster,colorType);
        }else if (colorType == 2){ // color palette
            ok = handlePaletteCase(rastersize, raster);
        }

        if (!ok)
            return false;

        IGeoReference georeference;
        double geosys[6];
        CPLErr err = gdal()->getGeotransform(_handle->handle(), geosys) ;
        if ( err == CE_None) {
            double a1 = geosys[0];
            double b1 = geosys[3];
            double a2 = geosys[1];
            double b2 = geosys[5];
            Coordinate crdLeftup( a1 , b1);
            Coordinate crdRightDown(a1 + rastersize.xsize() * a2, b1 + rastersize.ysize() * b2 ) ;
            cMin = Coordinate( min(crdLeftup.x, crdRightDown.x), min(crdLeftup.y, crdRightDown.y));
            cMax = Coordinate( max(crdLeftup.x, crdRightDown.x), max(crdLeftup.y, crdRightDown.y));

            if(!georeference.prepare(_resource.url().toString()))
                return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",raster->name() );
        } else {
            int iNrTiePoints = gdal()->getGCPCount(_handle->handle());
            if (iNrTiePoints > 0) {
                const GDAL_GCP* amtp = gdal()->getGCPs(_handle->handle());
                Envelope envTieLimits;
                for (int i = 0; i < iNrTiePoints; i++) {
                    Coordinate crdtiep(amtp[i].dfGCPX,amtp[i].dfGCPY,0);
                    envTieLimits += crdtiep;
                }
                cMin = envTieLimits.min_corner();
                cMax = envTieLimits.max_corner();
                if(!georeference.prepare(_resource.url().toString()))
                    return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",raster->name() );
            } else {
                cMin = Coordinate( 0, 0 );
                cMax = Coordinate( rastersize.xsize() - 1, rastersize.ysize() - 1);
                if(!georeference.prepare("code=georef:undetermined"))
                    return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Georeference",raster->name() );
                georeference->coordinateSystem(raster->coordinateSystem()); // the grf.prepare() for internal ilwis georeferences (among others "undetermined") does not autmatically set its csy
                georeference->envelope(Envelope(cMin, cMax));
            }
        }

        georeference->size(rastersize);
        georeference->compute();

        raster->envelope(Envelope(cMin, cMax));
        raster->coordinateSystem()->envelope(raster->envelope());
        raster->georeference(georeference);
        //if (!raster->gridRef()->prepare(raster,rastersize))
        //    return false;

        return true;

    }else{
        return ERROR2(ERR_INVALID_PROPERTY_FOR_2,"non-RasterCoverage",_fileUrl.toLocalFile());
    }
}

bool RasterCoverageConnector::handlePaletteCase(Size<> &rastersize, RasterCoverage* raster) {

    auto layerHandle = gdal()->getRasterBand(_handle->handle(), 1);
    auto paletteHandle = gdal()->getColorPalette(layerHandle);
    ColorPalette *palette = new ColorPalette();

    if (!paletteHandle)
        return false;
    int count = gdal()->getColorPaletteSize(paletteHandle);
    if ( count == 0)
        return false;
    ColorRangeBase::ColorModel model;
    GDALPaletteInterp colorType = gdal()->getPaletteColorInterpretation(paletteHandle);
    for(int i = 0; i < count; ++i) {

        GDALColorEntry *entry = gdal()->getColorPaletteEntry(paletteHandle, i);
        if ( !entry)
            continue;
        QColor clr;
        switch ( colorType){
        case GPI_RGB:
            clr.setRgb(entry->c1, entry->c2, entry->c3);
            model = ColorRangeBase::cmRGBA;
            break;
        case GPI_HLS:
            clr.setHsl(entry->c1, entry->c2, entry->c3); break;
            model = ColorRangeBase::cmHSLA;
            break;
        case GPI_CMYK:
            clr.setCmyk(entry->c1, entry->c2, entry->c3, entry->c4);
            model = ColorRangeBase::cmCYMKA;
            break;
        case GPI_Gray:
            clr.setRgb(entry->c1, entry->c1, entry->c1);
            model = ColorRangeBase::cmGREYSCALE;
        }
        clr.setAlpha(entry->c4);
        palette->add(new ColorItem(clr));

    }
    palette->defaultColorModel(model);

    _typeSize = 1;
    _gdalValueType = gdal()->rasterDataType(layerHandle);
    raster->datadefRef() = DataDefinition(IDomain("colorpalette"), reinterpret_cast<Range *>(palette));

    return true;
}

bool RasterCoverageConnector::handleColorCase(const Size<> &rastersize, RasterCoverage* raster, GDALColorInterp colorType){


    if ( colorType >= 3 && colorType <=5)
        _colorModel = ColorRangeBase::cmRGBA;
    else if ( colorType >= 7 && colorType <= 9)
        _colorModel = ColorRangeBase::cmHSLA;
    else
        _colorModel = ColorRangeBase::cmCYMKA;

     for(int layer =1 ; layer < rastersize.zsize(); ++layer){
         auto layerHandle = gdal()->getRasterBand(_handle->handle(), 1);
        GDALColorInterp ctype = gdal()->colorInterpretation(layerHandle);
        if ( ctype == GCI_AlphaBand){
            _hasTransparency = true;
            break;
        }
     }
    quint32 noOfComponents = _hasTransparency ? 4 : 3;
    std::map<int, int> vminRasterAll, vmaxRasterAll;
    for(int layer =0 ; layer < rastersize.zsize(); layer+=noOfComponents){
        std::map<int, int> vminRaster;
        std::map<int, int> vmaxRaster;
        for(int component = 0; component < noOfComponents; ++component){
            auto layerHandle = gdal()->getRasterBand(_handle->handle(), layer + component + 1);
            if (!layerHandle) {
                return ERROR2(ERR_COULD_NOT_LOAD_2, raster->name(),"layer");
            }
            GDALColorInterp ctype = gdal()->colorInterpretation(layerHandle);

            int ok;
            _gdalValueType = gdal()->rasterDataType(layerHandle);
            auto vmin = gdal()->minValue(layerHandle, &ok);
            auto vmax = gdal()->maxValue(layerHandle, &ok);
            vminRaster[ctype] = Ilwis::min(vmin, component == 0 ? 1000 : vminRaster[ctype]);
            vmaxRaster[ctype] = Ilwis::max(vmax, component == 0 ? -1000 : vmaxRaster[ctype]);
        }
        raster->datadefRef(layer / noOfComponents) = createDataDefColor(vminRaster, vmaxRaster);
        for(auto component =  vminRaster.begin(); component != vminRaster.end(); ++component)
            vminRasterAll[component->first] = std::min(component == vminRasterAll.begin() ? 1000 : vminRasterAll[component->first], vminRaster[component->first]);
        for(auto component =  vmaxRaster.begin(); component != vmaxRaster.end(); ++component)
            vmaxRasterAll[component->first] = std::max(component == vmaxRasterAll.begin() ? -1000 : vmaxRasterAll[component->first], vmaxRaster[component->first]);
    }

    raster->datadefRef() = createDataDefColor(vminRasterAll, vmaxRasterAll);

    return true;
}


DataDefinition RasterCoverageConnector::createDataDefColor(std::map<int, int>& vminRaster, std::map<int, int>& vmaxRaster){
    QColor clrMin, clrMax;
    switch(_colorModel){
        case ColorRangeBase::cmRGBA:
        clrMin = QColor(vminRaster[GCI_RedBand],vminRaster[GCI_GreenBand], vminRaster[GCI_BlueBand]);
        clrMax = QColor(vmaxRaster[GCI_RedBand],vmaxRaster[GCI_GreenBand], vmaxRaster[GCI_BlueBand]);
        if ( _hasTransparency)
            clrMin.setAlpha(vminRaster[GCI_AlphaBand]);
        break;
    case ColorRangeBase::cmHSLA:
        clrMin.setHsl(vminRaster[GCI_HueBand],vminRaster[GCI_SaturationBand], vminRaster[GCI_LightnessBand]);
        clrMin.setHsl(vmaxRaster[GCI_HueBand],vmaxRaster[GCI_SaturationBand], vmaxRaster[GCI_LightnessBand]);
        if ( _hasTransparency)
            clrMin.setAlpha(vminRaster[GCI_AlphaBand]);
        break;
    case ColorRangeBase::cmCYMKA:
        break;
    default:
        break;

    }
    Range *colorRange = new ContinuousColorRange( clrMin, clrMax, _colorModel);
    _typeSize = 1;
    _gdalValueType = GDT_Byte;
    return DataDefinition(IDomain("color"), colorRange);

}
bool RasterCoverageConnector::handleNumericLayerCase(int layer, RasterCoverage* raster) {

    auto layerHandle = gdal()->getRasterBand(_handle->handle(), layer+1);
    if (!layerHandle) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, raster->name(),"layer");
    }

    _gdalValueType = gdal()->rasterDataType(layerHandle);
    double resolution =  _gdalValueType <= GDT_Int32 ? 1 : 0;
    int accurateMin;
    int accurateMax;
    auto vmin = gdal()->minValue(layerHandle, &accurateMin);
    if (std::isinf(vmin) || std::isinf(-vmin))
        vmin = rUNDEF;
    auto vmax = gdal()->maxValue(layerHandle, &accurateMax);
    if (std::isinf(vmax) || std::isinf(-vmax))
        vmax = rUNDEF;    

    raster->datadefRef(0) = createDataDef(vmin, vmax, resolution, accurateMin && accurateMax, _offsetScales[0]);

    if ( !accurateMin || !accurateMax)
        raster->datadefRef() = DataDefinition(raster->datadef(0).domain(), new NumericRange());
    else
        raster->datadefRef() = DataDefinition(raster->datadef(0).domain(), new NumericRange(vmin, vmax, resolution));
    _typeSize = gdal()->getDataTypeSize(_gdalValueType) / 8;

    return true;
}


bool RasterCoverageConnector::handleNumericCase(const Size<> &rastersize, RasterCoverage* raster) {

    double vminRaster=rUNDEF, vmaxRaster=rUNDEF;
    double resolution= 0;
    for(int i =0 ; i < rastersize.zsize(); ++i){
        auto layerHandle = gdal()->getRasterBand(_handle->handle(), i+1);
        if (!layerHandle) {
            return ERROR2(ERR_COULD_NOT_LOAD_2, raster->name(),"layer");
        }

        int accurateMin;
        int accurateMax;
        _gdalValueType = gdal()->rasterDataType(layerHandle);
        resolution =  _gdalValueType <= GDT_Int32 ? 1 : 0;
        auto vmin = gdal()->minValue(layerHandle, &accurateMin);
        if (std::isinf(vmin) || std::isinf(-vmin))
            vmin = rUNDEF;
        auto vmax = gdal()->maxValue(layerHandle, &accurateMax);
        if (std::isinf(vmax) || std::isinf(-vmax))
            vmax = rUNDEF;

        if (sourceRef().hasProperty("scale") && sourceRef().hasProperty("offset")) {
            _offsetScales[i].offset = sourceRef()["offset"].toDouble();
            _offsetScales[i].scale = sourceRef()["scale"].toDouble();
        }

        if ( !accurateMin || !accurateMax)
            raster->datadefRef(i) = createDataDef(vmin, vmax, resolution, false, _offsetScales[i]);
        else {
            vminRaster = Ilwis::min(vmin, vminRaster);
            vmaxRaster = Ilwis::max(vmax, vmaxRaster); // Note that potentially this gives a valid NumericRange to a multiband RasterCoverage if one of the bands have a valid range (while other bands don't have one). But it is unlikely to encounter such data. The goal of keeping the total range invalid is so we can test on it, in order to decide whether we need to compute the statistics() at a later stage.
            raster->datadefRef(i) = createDataDef(vmin, vmax, resolution, true, _offsetScales[i]);
        }
    }
    createRasterDataDef(vminRaster, vmaxRaster, resolution, raster);
    _typeSize = gdal()->getDataTypeSize(_gdalValueType) / 8;  

    return true;

}

void RasterCoverageConnector::createRasterDataDef(double vminRaster, double vmaxRaster, double resolution, RasterCoverage* raster)
{
    if (_offsetScales[0].offset != rUNDEF && _offsetScales[0].scale != rUNDEF) {
        vminRaster = vminRaster * _offsetScales[0].scale  + _offsetScales[0].offset ;
        vmaxRaster = vmaxRaster * _offsetScales[0].scale  + _offsetScales[0].offset ;
    }

    raster->datadefRef() = DataDefinition(raster->datadef(0).domain(), new NumericRange(vminRaster, vmaxRaster, resolution));
}

DataDefinition RasterCoverageConnector::createDataDef(double vmin, double vmax, double resolution, bool accurate, GdalOffsetScale gdalOffsetScale){

    if (gdalOffsetScale.offset != rUNDEF && gdalOffsetScale.scale != rUNDEF) {
        vmin = vmin * gdalOffsetScale.scale  + gdalOffsetScale.offset ;
        vmax = vmax * gdalOffsetScale.scale  + gdalOffsetScale.offset ;
    }

    QString domName = NumericDomain::standardNumericDomainName(vmin, vmax,  resolution);
    IDomain dom;
    dom.prepare(domName);
    if(!dom.isValid()) {
        ERROR1(ERR_FIND_SYSTEM_OBJECT_1, domName);
        return DataDefinition();
    }
    DataDefinition def;
    def.domain(dom);
    if (accurate)
        def.range(new NumericRange(vmin, vmax, dom->range<NumericRange>()->resolution()));
    else
        def.range(new NumericRange()); // invalid NumericRange, force computing raster statistics
    return def;
}

inline double RasterCoverageConnector::value(char *block, int index) const{
    double v = rUNDEF;
    char *c = &(block[index * _typeSize]);
    switch (_gdalValueType) {
    case GDT_Byte:
        v = (quint8)*c; break;
    case GDT_Int16:
        v =  *(qint16 *)c; break;
    case GDT_UInt16:
        v =  *(quint16 *)c; break;
    case GDT_Int32:
        v = *(qint32 *)c; break;
    case GDT_UInt32:
        v = *(quint32 *)c; break;
    case GDT_Float32:
        v = *(float *)c; break;
    case GDT_Float64:
        v = *(double *)c; break;
    default:
        v = rUNDEF;
    }
    return v;
}

void RasterCoverageConnector::setColorValues(GDALColorInterp colorType, std::vector<double>& values, quint32 noItems, char *block) const
{
    for(quint32 i=0; i < noItems; ++i) {
        quint64 colorpart= value(block, i);
        switch (colorType){
        case GCI_RedBand:
        case GCI_HueBand:
            values[i] = ( colorpart << 16) | ( _hasTransparency ? 0 : 0xFF000000); break;
        case GCI_GreenBand:
        case GCI_SaturationBand:
            values[i] = ((quint32)values[i]) | ( colorpart << 8);break;
        case GCI_BlueBand:
        case GCI_LightnessBand:
            values[i] = ((quint32)values[i]) | colorpart;
        case GCI_AlphaBand:
             values[i] = ((quint32)values[i]) | ( colorpart << 24);break;
        default:
            //TODO other cases but without proper data a bit hard to do
            break;
        }
    }
}

void RasterCoverageConnector::readData(UPGrid& grid, GDALRasterBandH layerHandle, int inLayerBlockIndex, quint32 linesPerBlock, char *block, quint64 linesLeft) const
{
    CPLErr err;
    if ( linesLeft > linesPerBlock)
        err = gdal()->rasterIO(layerHandle,GF_Read,0,inLayerBlockIndex * linesPerBlock,grid->size().xsize(), linesPerBlock,
                         block,grid->size().xsize(), linesPerBlock,_gdalValueType,0,0 );
    else {
        err = gdal()->rasterIO(layerHandle,GF_Read,0,inLayerBlockIndex * linesPerBlock,grid->size().xsize(), linesLeft,
                         block,grid->size().xsize(), linesLeft,_gdalValueType,0,0 );
    }
    if ( err != CP_NONE ){
        QString message( gdal()->getLastErrorMsg());
        if ( message != "")
            kernel()->issues()->log("GDAL error :"+  message, IssueObject::itWarning);
    }
}

bool RasterCoverageConnector::moveIndexes(quint32& linesPerBlock, quint64& linesLeft, int& inLayerBlockIndex)
{
    ++inLayerBlockIndex;
    if ( linesLeft < linesPerBlock )
        return false;
    linesLeft -= linesPerBlock;

    return true;
}

bool RasterCoverageConnector::loadData(IlwisObject* data, const IOOptions& options ){
    quint32 bandindex = sourceRef().hasProperty("bandindex") ? sourceRef()["bandindex"].toUInt(): iUNDEF;
    auto layerHandle = gdal()->getRasterBand(_handle->handle(), bandindex != iUNDEF ? bandindex + 1 : 1);
    if (!layerHandle) {
        ERROR2(ERR_COULD_NOT_LOAD_2, "GDAL","layer");
        return false;
    }
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

    UPGrid& grid = raster->gridRef();

    quint32 linesPerBlock = grid->maxLines();
    qint64 blockSizeBytes = grid->blockSize(0) * _typeSize;
    char *block = new char[blockSizeBytes];
    quint64 totalLines =grid->size().ysize();
    std::map<quint32, std::vector<quint32> > blocklimits;

    //blocklimits; key = band number, value= blocks needed from this band
    if ( bandindex == iUNDEF)
        blocklimits = grid->calcBlockLimits(options);
    else{
        for(int i = 0; i < std::ceil((double)totalLines / linesPerBlock); ++i)
            blocklimits[bandindex].push_back(i);
    }

    for(const auto& layer : blocklimits){
        quint64 linesLeft = totalLines - grid->maxLines() * layer.second[0]; //std::min((quint64)grid->maxLines(), totalLines - grid->maxLines() * layer.second[0]);
        if ( _colorModel == ColorRangeBase::cmNONE || raster->datadef().domain()->valueType() == itPALETTECOLOR){ // palette entries are just integers so we can use the numeric read for it
            layerHandle = gdal()->getRasterBand(_handle->handle(), layer.first + 1);
            int inLayerBlockIndex = layer.second[0] % grid->blocksPerBand(); //
            for(const auto& index : layer.second) {
                quint32 offsetIndex = bandindex == iUNDEF ? layer.first : (layer.first - bandindex);
                loadNumericBlock(layerHandle, index, inLayerBlockIndex, linesPerBlock, linesLeft, block, raster,offsetIndex );

                if(!moveIndexes(linesPerBlock, linesLeft, inLayerBlockIndex))
                    break;
            }
        }else { // continous colorcase, combining 3/4 (gdal)layers into one
            int inLayerBlockIndex = layer.second[0] % grid->blocksPerBand(); //
            for(const auto& index : layer.second) {
                loadColorBlock(layer.first,index,inLayerBlockIndex,linesPerBlock,linesLeft,block,grid);

                if(!moveIndexes(linesPerBlock, linesLeft, inLayerBlockIndex)) // ensures that the administration with respect how much needs to be done is inorder
                    break;
            }
        }
    }

    delete [] block;
    _binaryIsLoaded = true;
    return true;
}

void RasterCoverageConnector::loadColorBlock(quint32 ilwisLayer, quint32 index, quint32 inLayerBlockIndex, quint32 linesPerBlock, quint64 linesLeft,char *block, UPGrid& grid) const{
    std::vector<double> values;
    // ilwis color layers consist of 3 or 4 gdal layers
    quint32 noOfComponents = _hasTransparency ? 4 : 3; // do we have a transparency layer?
    for( int component = 0; component < noOfComponents ; ++component){
        auto layerHandle = gdal()->getRasterBand(_handle->handle(), noOfComponents * ilwisLayer + component + 1);
        GDALColorInterp colorType = gdal()->colorInterpretation(layerHandle);
        readData(grid, layerHandle, inLayerBlockIndex, linesPerBlock, block, linesLeft);

        quint32 noItems = grid->blockSize(index);
        if ( noItems == iUNDEF)
            return ;
        if ( values.size() == 0)
            values.resize(noItems);
        setColorValues(colorType, values, noItems, block);
    }
    grid->setBlockData(index, values);
}

void RasterCoverageConnector::loadNumericBlock(GDALRasterBandH layerHandle,
                                               quint32 index,
                                               quint32 inLayerBlockIndex,
                                               quint32 linesPerBlock,
                                               quint64 linesLeft,
                                               char *block, RasterCoverage *raster,
                                               int bandIndex) const {
    UPGrid& grid = raster->gridRef();
    readData(grid, layerHandle, inLayerBlockIndex, linesPerBlock, block, linesLeft);
    int ok;
    const double nodata = gdal()->getUndefinedValue(layerHandle,&ok);
    quint32 noItems = grid->blockSize(index);
    if ( noItems == iUNDEF)
        return ;
    std::vector<double> values(noItems, rUNDEF);
    bool hasScaleOffset = _offsetScales[bandIndex].offset != rUNDEF && _offsetScales[bandIndex].scale != rUNDEF;
    for(quint32 i=0; i < noItems; ++i) {
        double v = value(block, i);
        if ( std::isnan(v) || std::isinf(v) )
            continue;

        if (hasScaleOffset)
            v = v * _offsetScales[bandIndex].scale + _offsetScales[bandIndex].offset;

        if ( ok == 0)
            values[i] = v;
        else
            values[i] = (ok && (nodata == v))  ? rUNDEF : v;
    }
    grid->setBlockData(index, values);
}

bool RasterCoverageConnector::setGeotransform(RasterCoverage *raster,GDALDatasetH dataset) {
    if ( raster->georeference()->grfType<CornersGeoReference>()) {
        //std::vector<double> sup = raster->georeference()->as<CornersGeoReference>()->support();
        Envelope env = raster->georeference()->envelope();
        Size <> sz = raster->georeference()->as<CornersGeoReference>()->size();
        double a2 = (env.max_corner().x - env.min_corner().x) / sz.xsize();
        double b2 = (env.max_corner().y - env.min_corner().y) / sz.ysize();
        double geoTransform[6] = { env.min_corner().x, a2, 0, env.max_corner().y, 0, -b2 };

        CPLErr err = gdal()->setGeoTransform(dataset,geoTransform);
        if ( err != CE_None) {
            reportError(dataset);
            return false;
        }
        return true;
    }
    //return ERROR2(ERR_OPERATION_NOTSUPPORTED2,TR("Georeference type"), "Gdal");
    return true;
}

bool RasterCoverageConnector::loadDriver()
{
    _driver = gdal()->getGDALDriverByName(_gdalShortName.toLocal8Bit());
    if ( !_driver ) {
        return ERROR2(ERR_COULD_NOT_LOAD_2, "data-source", _fileUrl.toString());
    }
    const char* metaitem = gdal()->getMetaDataItem(_driver, GDAL_DCAP_CREATE, NULL);
    if (QString(metaitem).toLower() != "yes") {
        return ERROR2(ERR_OPERATION_NOTSUPPORTED2, "write data-source", _fileUrl.toString());
    }

    return true;
}



bool RasterCoverageConnector::store(IlwisObject *obj, const IOOptions & )
{
    if(!loadDriver())
        return false;

    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);

    DataDefinition currentDef = raster->datadefRef();
    if (! hasType(raster->datadef().domain()->ilwisType(),itNUMERICDOMAIN | itCOLORDOMAIN)){
        IDomain dom;
        QString code = raster->datadef().domain()->ilwisType() == itITEMDOMAIN ? "code=count" : "code=value";
        if(!dom.prepare(code)) { //TODO:  for the moment only value maps in gdal
            return ERROR1(ERR_NO_INITIALIZED_1,obj->name());
        }
        currentDef.domain(dom);
    }
    Size<> sz = raster->size();
    GDALDataType gdalType = ilwisType2GdalType(currentDef.range()->valueType());
    QString filename = constructOutputName(_driver);
    bool isColorMap = currentDef.domain()->ilwisType() == itCOLORDOMAIN;
    bool ispaletteMap = currentDef.domain()->valueType() == itPALETTECOLOR;

    GDALDatasetH dataset = 0;
    if ( ispaletteMap && format() == "GTiff"){
        char *options[] = {"PHOTOMETRIC=PALETTE", NULL};
        dataset = gdal()->create( _driver, filename.toLocal8Bit(), sz.xsize(), sz.ysize(),  sz.zsize(), gdalType, options );
    }else
        dataset = gdal()->create( _driver, filename.toLocal8Bit(), sz.xsize(), sz.ysize(),  isColorMap ?  sz.zsize() * 3 : sz.zsize(), gdalType, 0 );
    if ( dataset == 0) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2, "data set",_fileUrl.toLocalFile());
    }
    bool ok = setGeotransform(raster, dataset);
    if (ok)
        ok = setSRS(raster, dataset);

    if (!ok)
        return false;

    if ( isColorMap ){
        ok = storeColorRaster(raster, dataset)    ;
    } else {
        switch(gdalType) {
        case GDT_Byte:
            ok = save<quint8>(raster, dataset,gdalType);break;
        case GDT_UInt16:
            ok = save<quint16>(raster, dataset,gdalType);break;
        case GDT_Int16:
            ok = save<qint16>(raster, dataset,gdalType);break;
        case GDT_Int32:
            ok = save<qint32>(raster, dataset,gdalType);break;
        case GDT_UInt32:
            ok = save<quint32>(raster, dataset,gdalType);break;
        case GDT_Float32:
            ok = save<float>(raster, dataset,gdalType);break;
        case GDT_Float64:
            ok = save<double>(raster, dataset,gdalType);break;
        default:
            ok= ERROR1(ERR_NO_INITIALIZED_1, "gdal Data type");
        }
    }

    gdal()->close(dataset);

    return ok;
}

bool RasterCoverageConnector::storeColorRaster(RasterCoverage *raster, GDALDatasetH dataset){
    bool ok = true;
    IlwisTypes tp = raster->datadef().domain()->valueType();
    if (tp == itCONTINUOUSCOLOR){
        quint32 gdalLayer = 1;

        for(int band = 0; band < raster->size().zsize() && ok; ++band){
            ok = saveByteBand(raster,dataset,gdalLayer++, band, GCI_RedBand);
            ok &= saveByteBand(raster,dataset,gdalLayer++, band, GCI_GreenBand);
            ok &= saveByteBand(raster,dataset,gdalLayer++, band, GCI_BlueBand);
        }
    } else { // palette case
        GDALColorTableH hpalette = gdal()->createColorPalette(GPI_RGB);
        if (!hpalette)
            return false;
        RangeIterator<QColor, ColorPalette> iter(raster->datadef().range<>().data());
        while(iter.isValid()){
            GDALColorEntry entry;
            QColor clr = *iter;
            entry.c1 = clr.red();
            entry.c2 = clr.green();
            entry.c3 = clr.blue();
            entry.c4 = clr.alpha();
            gdal()->setColorPaletteEntry(hpalette,iter.current(), &entry);
            ++iter;
        }
        GDALRasterBandH hband = gdal()->getRasterBand(dataset,1);
        if ( gdal()->setColorPalette(hband, hpalette) != CE_None){
            const char *err = gdal()->getLastErrorMsg();
            kernel()->issues()->log(err);
            return false;
        }
        ok = save<quint8>(raster, dataset,GDT_Byte);
    }
    return ok;
}

bool RasterCoverageConnector::saveByteBand(RasterCoverage *raster, GDALDatasetH dataset,int gdallayerindex, int band, GDALColorInterp colorType){
    quint32 columns = raster->size().xsize();
    PixelIterator iter = raster->band(band);
    std::vector<unsigned char> data(columns);
    GDALRasterBandH hband = gdal()->getRasterBand(dataset,gdallayerindex);
    if (!hband) {
        return ERROR1(ERR_NO_INITIALIZED_1,"raster band");
    }
    if ( gdal()->setColorInterpretation(hband,colorType) != CE_None) // not supported by this format
        return ERROR2(ERR_OPERATION_NOTSUPPORTED2,"Color"," this format");
    auto endPosition =  iter.end();
    while(iter != endPosition) {
        for_each(data.begin(), data.end(), [&](unsigned char& v){
           quint32 component = (quint32)*iter;
            switch(colorType){
            case GCI_RedBand:
                v = (0x00FF0000 & component) >> 16;break;
            case GCI_GreenBand:
                v =(0x0000FF00 & component) >> 8;break;
            case GCI_BlueBand:
                v = 0x00000000FF & component;break;
            default:
                break;
            }
            ++iter;
        });

        double y = iter.zchanged() ? iter.box().ylength()  : iter.position().y;

        if(iter == endPosition){
            y = iter.box().ylength();
        }

        gdal()->rasterIO(hband, GF_Write, 0, y - 1, columns, 1, (void *)&data[0],columns,1, GDT_Byte,0,0 );

    }
    return true;
}

bool RasterCoverageConnector::setSRS(Coverage *raster, GDALDatasetH dataset) const
{
    OGRSpatialReferenceH srsH = createSRS(raster->coordinateSystem());
    if ( srsH == 0) { // we are not setting CRS
       // reportError(dataset);
        return true;
    }
    char *wktText = NULL;
    gdal()->exportToWkt(srsH,&wktText);
    CPLErr err = gdal()->setProjection(dataset, wktText);
    gdal()->free(wktText);
    if ( err != CE_None) {
        reportError(dataset);
        return false;
    }
    return true;
}


void RasterCoverageConnector::reportError(GDALDatasetH dataset) const
{
    kernel()->issues()->log(QString(gdal()->getLastErrorMsg()));
    gdal()->close(dataset);
}


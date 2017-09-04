#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "domain.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "pixeliterator.h"
#include "grid.h"
#include "factory.h"
#include "abstractfactory.h"
#include "versioneddatastreamfactory.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "coverageserializerv1.h"
#include "rawconverter.h"
#include "rasterserializerv1.h"

using namespace Ilwis;
using namespace Stream;

RasterSerializerV1::RasterSerializerV1(QDataStream& stream) : CoverageSerializerV1(stream)
{
}

template<typename T> void storeBulk(const RawConverter& converter, QDataStream& stream, StreamConnector *streamconnector, const BoundingBox& box, const IRasterCoverage& raster){
    quint64 count = streamconnector->position();
    if ( streamconnector->isFileBased()){
        const UPGrid& grid = raster->grid();
        quint32 blockCount = grid->blocks();
        stream << blockCount;
        std::vector<T> rawData(grid->blockSize(0));
        for(quint32 i = 0; i < blockCount; ++i){
            quint64 blockSize = grid->blockSize(i);
            stream << i;
            stream << blockSize;
            for(int j = 0; j < blockSize; ++j)
                rawData[j] = converter.real2raw(grid->value(i,j));
            stream.writeRawData((const char *)rawData.data(), blockSize * sizeof(T));
        }
        streamconnector->flush(true);
    }else {
        PixelIterator iter(raster, box);
        while(iter != iter.end()){
            if ( count >= STREAMBLOCKSIZE - 9 ) {
                streamconnector->flush(false);
                count = 0;

            }
            count += sizeof(T);
            stream << (T)converter.real2raw(*iter);
            ++iter;
        }
    }
}

template<typename T> void loadBulk(const RawConverter& converter, QDataStream& stream, StreamConnector *streamconnector, const BoundingBox& box, const IRasterCoverage& raster){

    if ( streamconnector->isFileBased()){
        //const UPGrid& grid = raster->grid();
        quint32 blockCount;
        stream >> blockCount;
        if ( !box.isNull() && box.isValid()){
            //for the moment we only accept whole layers as possible subsets, might improve this in the future
            int layer = box.min_corner().z ;
            blockCount =  raster->grid()->blocksPerBand();
            int extraOffsets = blockCount * layer * (sizeof(quint32) + sizeof(qint64)); // per block there is an index and a blocksize - 12 bytes;
            int seekPos = stream.device()->pos() + extraOffsets + layer * (raster->size().xsize() * raster->size().ysize() * sizeof(T));
            stream.device()->seek(seekPos);

        }
        quint32 blockIndex;
        quint64 blockSize, initBlockSize;
        stream >> blockIndex;
        stream >> blockSize;
        initBlockSize = blockSize;

        std::vector<T> rawdata(initBlockSize);
        std::vector<double> realdata(initBlockSize);

        for(int i = 0; i < blockCount; ++i){

            stream.readRawData((char *)&rawdata[0],blockSize * sizeof(T) );
            for( int j = 0; j < blockSize; ++j)
                realdata[j] = converter.raw2real(rawdata[j]);
            raster->gridRef()->setBlockData(i, realdata);
            if ( i < blockCount - 1){
                stream >> blockIndex;
                stream >> blockSize;
                if ( blockSize != initBlockSize)
                    realdata.resize(blockSize);
            }

        }
    streamconnector->flush(true);
    } else {
     // TODO network restore, roughly code below though that was based on a older version
        //    UPGrid &grid = raster->gridRef();
        //    qint64 blockSizeBytes = grid->blockSize(block) * sizeof(T);
        //    qint64 szLeft = data.size();
        //    T value;
        //    QBuffer buf(&data);
        //    buf.open(QIODevice::ReadWrite);
        //    QDataStream stream(&buf);
        //    std::vector<double> values(grid->blockSize(0));
        //    quint32 noItems = grid->blockSize(block);
        //    if ( noItems == iUNDEF)
        //        return 0;

        //    values.resize(noItems);
        //    for(quint32 i=0; i < noItems; ++i) {
        //        stream >> value;

        //        values[i] = converter.raw2real(value);
        //    }

        //    grid->setBlockData(block, values);
        //    szLeft -= blockSizeBytes;
    }
}

bool RasterSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::store(obj, options))
        return false;
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);


    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    _stream << raster->size().xsize() << raster->size().ysize() << raster->size().zsize();


    if(!storeDataDefintion(raster->datadef(), _stream, options))
        return false;

    for(int index = 0; index < raster->size().zsize(); ++index)   {
        const DataDefinition& def = raster->datadef(index);
        storeDataDefintion(def,_stream, options);

    }

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    auto vtype = raster->stackDefinition().domain()->valueType();
    _stream << vtype;
    storeSystemPath(raster->stackDefinition().domain()->resource());
    domainStreamer->store( raster->stackDefinition().domain().ptr(), options);

    std::vector<QString> indexes = raster->stackDefinition().indexes();
    _stream << (quint32)indexes.size();
    for(auto index : indexes)
        _stream << index;

    std::unique_ptr<DataInterface> grfstreamer(factory->create(Version::interfaceVersion, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;

    storeSystemPath(raster->georeference()->resource());
    if(!grfstreamer->store(raster->georeference().ptr(), options))
        return false;
    _stream << raster->hasAttributes();
    if ( raster->hasAttributes()){
        std::unique_ptr<DataInterface> tblstreamer(factory->create(Version::interfaceVersion, itTABLE,_stream));
        if ( !tblstreamer)
            return false;

        if(!tblstreamer->store(raster->attributeTable().ptr(), options))
            return false;
        _stream << raster->primaryKey();
    }

    return true;

}

bool RasterSerializerV1::storeData(IlwisObject *obj, const IOOptions &options )
{
    qint64 pos = _stream.device()->pos();
    _stream << pos + sizeof(qint64);
    _stream << itRASTER;
    _stream << Version::interfaceVersion;
    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);
    RawConverter converter;
    if ( hasType(raster->datadef().domain()->ilwisType() , itNUMERICDOMAIN)){
        NumericStatistics& stats = raster->statistics(ContainerStatistics<double>::pBASIC);
        qint16 digits = stats.significantDigits();
        double scale = digits >= 10 ? 0 : std::pow(10,-digits);
        converter = RawConverter(stats[ContainerStatistics<double>::pMIN], stats[ContainerStatistics<double>::pMAX],scale);

        _stream << stats[ContainerStatistics<double>::pMIN] << stats[ContainerStatistics<double>::pMAX] << scale;
    }else{
        if(hasType(raster->datadef().domain()->ilwisType() ,itITEMDOMAIN) )
            converter = RawConverter("ident");
        if(hasType(raster->datadef().domain()->ilwisType() ,itCOLORDOMAIN) )
            converter = RawConverter("color");

    }
    if ( !converter.isValid()){
        kernel()->issues()->log(QString(TR("Couldnt find a correct converter for raster data of %1")).arg(obj->name()));
        return false;
    }
    BoundingBox box;
    if (options.contains("lines")) {
        QStringList parts = options["lines"].toString().split(" ");
        quint32 layer = parts[0].toUInt();
        quint32 minlines = parts[1].toUInt();
        quint32 maxlines = parts[2].toUInt();
        _stream <<  layer <<minlines << maxlines;
        box = BoundingBox(Pixel(0, minlines,layer), Pixel(raster->size().xsize(),maxlines,layer));

    }else {
        quint32 undef = iUNDEF;
        _stream << undef << undef << undef;
    }
    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        storeBulk<quint8>(converter, _stream, _streamconnector, box,rcoverage); break;
    case itINT16:
        storeBulk<qint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT16:
        storeBulk<quint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT32:
        storeBulk<qint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT32:
        storeBulk<quint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itDOUBLE:
        storeBulk<double>(converter, _stream, _streamconnector, box, rcoverage); break;
        break;
    case itINT64:
    default:
        for(double v : rcoverage)
            _stream << (qint64)v;
        break;
    }

    return true;
}

bool RasterSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!CoverageSerializerV1::loadMetaData(obj, options))
        return false;
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;

    RasterCoverage *raster = static_cast<RasterCoverage *>(obj);


    quint32 xsize, ysize, zsize;
    _stream >> xsize >> ysize >> zsize;
    raster->size(Size<>(xsize, ysize, zsize));


    loadDataDefinition(raster->datadefRef(),_stream, options);
    for(int band = 0; band < raster->size().zsize(); ++band) {
        loadDataDefinition(raster->datadefRef(band), _stream, options)    ;
    }
    IlwisTypes valueType;
    _stream >> valueType;
    quint64 type;
    QString version, url;
    _stream >> url;
    _stream >> type;
    _stream >> version;

    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;

    IDomain systemDomain = makeSystemObject<IDomain>(url);
    IDomain dom(type|valueType);
    domainStreamer->loadMetaData( dom.ptr(), options);
    quint32 nrOfBands;
    _stream >> nrOfBands;
    std::vector<QString> variants(nrOfBands);
    for(int i =0; i < nrOfBands; ++i){
        _stream >> variants[i];
    }
    raster->stackDefinitionRef().setSubDefinition(systemDomain.isValid() ? systemDomain : dom, variants);


    _stream >> url;
    _stream >> type;
    _stream >> version;

    std::unique_ptr<DataInterface> grfstreamer(factory->create(version, itGEOREF,_stream));
    if ( !grfstreamer)
        return false;
    IGeoReference systemGrf = makeSystemObject<IGeoReference>(url);
    IGeoReference georeference (type);
    grfstreamer->loadMetaData(georeference.ptr(), options)    ;
    raster->georeference(systemGrf.isValid() ? systemGrf : georeference);


    bool hasAttr;
    _stream >> hasAttr;
    if ( hasAttr){
        _stream >> type;
        _stream >> version;
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        std::unique_ptr<DataInterface> tableStreamer(factory->create(version, itTABLE,_stream));
        if ( !tableStreamer)
            return false;
        ITable tbl;
        tbl.prepare();

        tableStreamer->loadMetaData(tbl.ptr(),options);
        _stream >> type;
        _stream >> version;
        tableStreamer->loadData(tbl.ptr(),options);
        QString primkey;
        _stream >> primkey;
          raster->primaryKey(primkey);
        tbl->resourceRef().setExtendedType(itRASTER);
        tbl->resourceRef().addProperty("rasterid", raster->id());
        raster->setAttributes(tbl);
    }
    qint64 beginData;
    _stream >> beginData;
    _streamconnector->beginDataSection(beginData);

    return true;
}


bool RasterSerializerV1::loadData(IlwisObject *data, const IOOptions &options)
{
    RasterCoverage *raster = static_cast<RasterCoverage *>(data);
    BoundingBox box;
    RawConverter converter;
    if ( hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN)){
        double mmin, mmax, mscale;
        _stream >> mmin >> mmax >> mscale;
        converter = RawConverter(mmin, mmax, mscale);
    }else {
        if (  hasType(raster->datadef().domain()->ilwisType(), itITEMDOMAIN))
            converter = RawConverter("ident");
        if ( hasType(raster->datadef().domain()->ilwisType(), itCOLORDOMAIN))
            converter = RawConverter("color");
    }
    if ( !converter.isValid()){
        kernel()->issues()->log(QString(TR("Couldnt find a correct converter for raster data of %1")).arg(data->name()));
        return false;
    }

    quint32 layerIndex, minLines, maxLines; //only defined in some cases, if not defined it assumed that the whole coverage is there
    _stream >> layerIndex >> minLines >> maxLines;
    Resource resource = data->resource();
    if ( resource.code().indexOf("band=") == 0){
        bool ok;
        QString part = resource.code().mid(5);
        int band = part.toInt(&ok);
        if ( ok){
            Size<> sz = raster->size();
            box = BoundingBox(Pixel(0,0,band),Pixel(sz.xsize(),sz.ysize(),band));
        }
    }

    IRasterCoverage rcoverage(raster);
    switch (converter.storeType()){
    case itUINT8:
        loadBulk<quint8>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT16:
        loadBulk<qint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT16:
        loadBulk<quint16>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT32:
        loadBulk<qint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itUINT32:
        loadBulk<quint32>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itDOUBLE:
        loadBulk<double>(converter, _stream, _streamconnector, box, rcoverage); break;
    case itINT64:
    default:
            loadBulk<qint64>(converter, _stream, _streamconnector, box, rcoverage); break;
    }
    _dataLoaded = true;
    return true;
}

//quint32 RasterSerializerV1::loadGridBlock(IlwisObject *data, quint32 block, QByteArray &blockdata, const RawConverter& converter, const IOOptions &)
//{
//    RasterCoverage *raster = static_cast<RasterCoverage *>(data);

//    IRasterCoverage rcoverage(raster);
//    switch (converter.storeType()){
//    case itUINT8:
//        return loadBulk<quint8>(converter, block, blockdata, rcoverage); break;
//    case itINT16:
//        return loadBulk<qint16>(converter, block, blockdata, rcoverage); break;
//    case itINT32:
//        return loadBulk<qint32>(converter, block, blockdata, rcoverage); break;
//    case itINT64:{
//    default:
//            quint64 noOfPixels = raster->grid()->blockSize(block);
//            std::vector<double> values(noOfPixels);
//            for(int j = 0; j < noOfPixels; ++j){
//                _stream >> values[j];
//            }
//            raster->gridRef()->setBlockData(block, values);
//            return blockdata.size() - noOfPixels * 8;
//        }
//    }
//}

VersionedSerializer *RasterSerializerV1::create(QDataStream &stream)
{
    return new RasterSerializerV1(stream);
}

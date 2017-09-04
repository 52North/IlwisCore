#include <QBuffer>
#include <QColor>
#include "raster.h"
#include "version.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "featurecoverage.h"
#include "numericdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "catalog.h"
#include "thematicitem.h"
#include "interval.h"
#include "coloritem.h"
#include "identifierrange.h"
#include "intervalrange.h"
#include "colorrange.h"
#include "itemdomain.h"
#include "coordinatesystem.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "proj4parameters.h"
#include "conventionalcoordinatesystem.h"
#include "feature.h"
#include "factory.h"
#include "abstractfactory.h"
#include "rawconverter.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "location.h"
#include "workflownode.h"
#include "workflow.h"
#include "downloadmanager.h"
#include "analysispattern.h"
#include "applicationmodel.h"
#include "combinationmatrix.h"
#include "model.h"
#include "versioneddatastreamfactory.h"


using namespace Ilwis;
using namespace Stream;

ConnectorInterface *StreamConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new StreamConnector(resource, load, options);

}

IlwisObject *StreamConnector::create() const
{
    switch(_resource.ilwisType()){
    case itFEATURE:
        return new FeatureCoverage(_resource);
    case itRASTER:
        return new RasterCoverage(_resource);
    case itTABLE:
    case itFLATTABLE:
        return new FlatTable(_resource);
    case itNUMERICDOMAIN:
        return new NumericDomain(_resource);
    case itGEOREF:
        return new GeoReference(_resource);
    case itWORKFLOW:
        return new Workflow(_resource);
    case itCONVENTIONALCOORDSYSTEM:
        return new ConventionalCoordinateSystem(_resource);
     case itCATALOG:
        return new Catalog(_resource);
    case itMODEL:
        return new Model(_resource);
    case itCOMBINATIONMATRIX:
        return new CombinationMatrix(_resource) ;
    case itITEMDOMAIN:{
        if ( hasType(_resource.extendedType(), itNUMERICITEM))
            return new IntervalDomain(_resource);
        if ( hasType(_resource.extendedType(), itIDENTIFIERITEM))
            return new NamedIdDomain(_resource);
        if ( hasType(_resource.extendedType(), itTHEMATICITEM))
            return new ThematicDomain(_resource);
        if ( hasType(_resource.extendedType(), itPALETTECOLOR))
            return new ItemDomain<ColorItem>(_resource);

    }
    default:
        return 0;
    }
}


StreamConnector::StreamConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource,load,options)
{
    QUrlQuery query(sourceRef().url());
    QString url =QString(sourceRef().url(true).toString(QUrl::RemoveQuery));
    if ( query.hasQueryItem("datasource")){
        QString item = query.queryItemValue("datasource");
        url += "/" + item;
        sourceRef().setUrl(url);
    }
    if ( url.indexOf(".ilwis") == -1){
        url += ".ilwis";
        sourceRef().setUrl(url);
        sourceRef().setUrl(url,true);
    }
}

StreamConnector::~StreamConnector()
{

}

bool StreamConnector::loadMetaData(IlwisObject *object, const IOOptions &options)
{
    if ( _resource.url(true).scheme() == "file"){
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        if (!openSource(true))
            return false;
        QDataStream stream(_datasource.get());
        int streamversion;
        stream >> streamversion;
        stream.setVersion(streamversion);
        IlwisTypes tp;
        QString version;
        stream >> tp;
        stream >> version;
        std::unique_ptr<VersionedSerializer> serializer(factory->create(version,source().ilwisType(),stream));
        if (!serializer)
            return false;
        serializer->connector(this);
        bool ok = serializer->loadMetaData(object,options);
        if ( !hasType(object->ilwisType(), itCOVERAGE | itTABLE)) // apart from these one the rest has no binaries, so we never need to load it
            _binaryIsLoaded = true;
        return ok;

    }else {
        DownloadManager manager(_resource);
        return manager.loadMetaData(object,options);
    }

}

bool StreamConnector::loadData(IlwisObject *object, const IOOptions &options){
    if ( _resource.url(true).scheme() == "file"){
        //QFileInfo inf(_resource.url().toLocalFile());
        VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
        QDataStream stream(_datasource.get());

        IlwisTypes tp;
        QString version;
        if ( _beginDataSection != -1)
            stream.device()->seek(_beginDataSection);
        stream >> tp;
        stream >> version;
        std::unique_ptr<VersionedSerializer> serializer(factory->create(version, source().ilwisType(),stream));
        if (!serializer)
            return false;
        serializer->connector(this);
        _binaryIsLoaded = serializer->loadData(object,options);
        return _binaryIsLoaded;

    }else {
        DownloadManager manager(_resource);
        return manager.loadData(object,options);
    }
}

bool StreamConnector::openSource(bool reading){
    QUrl url = _resource.url(true);
    if ( url.scheme() == "http")
    {
        _bytes.resize(STREAMBLOCKSIZE);
        _bytes.fill(0);
        QBuffer *buf = new QBuffer(&_bytes);
        if (!buf->open(QIODevice::ReadWrite))
            return false;
        _datasource.reset(buf);
        return true;
    }
    QString scheme = url.scheme();
    if ( scheme == "file"){
        QString filename = url.toLocalFile();
        QFileInfo inf(filename);
        if ( inf.suffix() != "ilwis"){
            filename = inf.absolutePath() + "/" + inf.baseName() + ".ilwis";
            QString correctUrl = QUrl::fromLocalFile(filename).toString();
            // we remove the resource for the moment from the mastercatalog as it will de re-added under a different name; else we get two references to it
            mastercatalog()->removeItems({source()});
            sourceRef().setUrl(correctUrl,false,false);
            sourceRef().setUrl(correctUrl,true,false);
            mastercatalog()->addItems({source()});
        }
        QFile *file = new QFile(filename);

        if ( reading){
            if ( !file->exists()){
                delete file;
                return ERROR1(ERR_MISSING_DATA_FILE_1,filename);
            }
        }
        if (file->open(reading  ? QIODevice::ReadOnly : (QIODevice::ReadWrite | QIODevice::Truncate))) {
            _datasource.reset(file);
            return true;
        }
    }
    return false;
}


bool StreamConnector::store(IlwisObject *obj, const IOOptions &options){
    if (!openSource(false))
        return false;
    QDataStream stream(_datasource.get());
    stream << stream.version();

    const VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (factory){
        _versionedConnector.reset( factory->create(Version::interfaceVersion,_resource.ilwisType(),stream));
    }

    if (!_versionedConnector)
        return false;
    _versionedConnector->connector(this);
    bool ok;
    int storemode = options.contains("storemode") ? options["storemode"].toInt() : IlwisObject::smMETADATA | IlwisObject::smBINARYDATA;
    if ( hasType(storemode, IlwisObject::smMETADATA))
        ok = _versionedConnector->store(obj, options);
    if ( hasType(storemode, IlwisObject::smBINARYDATA))
        ok = _versionedConnector->storeData(obj, options);

    flush(true);

    _datasource->close();
    _versionedConnector.reset(0);

    return ok;

}

QString StreamConnector::provider() const
{
    return "Stream";
}

bool StreamConnector::needFlush() const
{
    return _datasource->pos() >= STREAMBLOCKSIZE && _resource.url().scheme() != "file";
}

quint32 StreamConnector::position() const {
    if ( _datasource)
        return _datasource->pos();
    return iUNDEF;
}

void StreamConnector::flush(bool last)
{
    if ( _resource.url().scheme() == "file") // we dont flush with files; OS does this
        return;
    QBuffer *buf = static_cast<QBuffer *>(_datasource.get());
    emit dataAvailable(buf,last);

}

bool StreamConnector::isReadOnly() const
{
    return false;
}

bool StreamConnector::isFileBased() const
{
    return sourceRef().url(true).scheme() == "file";
}

bool StreamConnector::dataIsLoaded() const
{
    return _binaryIsLoaded;
}

QString StreamConnector::format() const
{
    return IlwisObject::type2Name(source().ilwisType());
}

qint64 StreamConnector::beginDataSection() const
{
    return _beginDataSection;
}

void StreamConnector::beginDataSection(qint64 begin)
{
    _beginDataSection = begin;
}

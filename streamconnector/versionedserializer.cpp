#include "kernel.h"
#include "ilwisdata.h"
#include "version.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "factory.h"
#include "abstractfactory.h"
#include "domain.h"
#include "datadefinition.h"
#include "versioneddatastreamfactory.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "operationhelper.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer::VersionedSerializer(QDataStream& stream) : _stream(stream)
{
}

VersionedSerializer::~VersionedSerializer()
{

}

bool VersionedSerializer::loadItems(std::vector<Resource> &){
    QString var;
    _stream >> var;
    _stream >> var;
    _stream >> var;
    bool readonly;
    _stream >> readonly;
    double time;
    _stream >> time;
    _stream >> time;

    return true;
}

bool VersionedSerializer::dataIsLoaded() const
{
    return _dataLoaded;
}

bool VersionedSerializer::loadMetaData(IlwisObject *obj, const IOOptions &)
{
    if ( obj->isReadOnly())
        return false;
    bool skip =  obj->resource().isValid();
    skip = skip && hasType(obj->ilwisType(), itCATALOG | itCOVERAGE);
    // we are not going to replace info in the resource if not needed.
    // reason is that this can be a container object (e.g multiband raster) and we are reading here the container
    // and not the actual object embedded in the container
 IlwisTypes tp;
    _stream >> tp;
    if (!skip)
        obj->extendedType(tp);
    QString var,code;
    _stream >> var;
    _stream >> code;
    if (!skip)
        obj->code(code);
    if (!skip )
        obj->name(var);

    _stream >> var;
    if (!skip)
        obj->setDescription(var);
    bool readonly;
    _stream >> readonly;
    if (!skip)
        obj->readOnly(readonly);


    double time;
    _stream >> time;
    if (!skip)
        obj->modifiedTime(time);
    _stream >> time;
    if (!skip)
        obj->createTime(time);

    return true;

}

bool VersionedSerializer::store(IlwisObject *obj, const IOOptions &options)
{
    double mtime = (double)obj->modifiedTime();
    double ctime = (double)obj->createTime();
    _stream <<  obj->ilwisType() << Version::interfaceVersion << obj->extendedType() << obj->name() << obj->code() << obj->description() << obj->isReadOnly()  << mtime << ctime;

    return true;
}

void VersionedSerializer::connector(StreamConnector *streamconnector)
{
    _streamconnector = streamconnector;
}


bool VersionedSerializer::storeDataDefintion(const DataDefinition &def, QDataStream &stream, const IOOptions &options) const
{
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    std::unique_ptr<DataInterface> domainStreamer(factory->create(Version::interfaceVersion, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;
    _stream << def.domain()->valueType();
    storeSystemPath(def.domain()->resource());
    domainStreamer->store(def.domain().ptr(), options);
    if ( !def.range().isNull()) // no range for textdomains
        def.range()->store(_stream);

    return true;
}

bool VersionedSerializer::loadDataDefinition(DataDefinition &def, QDataStream &stream,const IOOptions &options)
{
    VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
    if (!factory)
        return false;
    IlwisTypes valueType, type;
    QString version, url;
    _stream >> valueType;
    _stream >> url;
    _stream >> type;
    _stream >> version;
    std::unique_ptr<DataInterface> domainStreamer(factory->create(version, itDOMAIN,_stream));
    if ( !domainStreamer)
        return false;

    IDomain systemDomain = makeSystemObject<IDomain>(url);
    IDomain dom(type | valueType);
    Range *range = 0;
    domainStreamer->loadMetaData(dom.ptr(), options);
    if ( type != itTEXTDOMAIN){
        range = Range::create(dom->valueType());
        if (!range)
            return false;
        range->load(_stream);
    }
    def = DataDefinition(systemDomain.isValid() ? systemDomain : dom,range);

    return true;
}

void VersionedSerializer::storeSystemPath(const Resource &resource) const
{
    QString path = resource.url().toString();
    if ( path.indexOf("ilwis://system") == 0)
        _stream << resource.url().toString(); // this string is (potentially) only usefull if it is a system object.
    else
        _stream << QString(sUNDEF);
}

bool VersionedSerializer::store(const QString& v, IlwisTypes valueType,const IOOptions &options){

    if ( hasType(valueType, itILWISOBJECT)){
        bool storeall = options.contains("storeall") ? options["storall"].toBool() : false;
        _stream << storeall;
        if ( storeall){
            VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
            std::unique_ptr<DataInterface>  streamer(factory->create(Version::interfaceVersion, valueType,_stream));
            if ( !streamer)
                return false;
            IIlwisObject obj;
            obj.prepare(v, valueType);
            if ( obj.isValid())
                return streamer->store( obj.ptr(), options);
            return true;
        }
        _stream << v;
    }else {
        _stream << v;
    }
    return true;
}

bool VersionedSerializer::loadMetaData(const Ilwis::IOOptions &options, IlwisTypes tp, QString &v)
{
    if ( hasType(tp, itILWISOBJECT)){
        bool storeall;
        _stream >> storeall;
        if ( storeall){
            VersionedDataStreamFactory *factory = kernel()->factory<VersionedDataStreamFactory>("ilwis::VersionedDataStreamFactory");
            std::unique_ptr<DataInterface>  streamer(factory->create(Version::interfaceVersion, tp,_stream));
            if ( !streamer)
                return false;
            IIlwisObject obj ;// TODO : this wwont work :)
            return streamer->loadMetaData( obj.ptr(), options);
        }else {
            _stream >> v;
            return true;
        }
    }else {
        _stream >> v;
    }
    return true;
}



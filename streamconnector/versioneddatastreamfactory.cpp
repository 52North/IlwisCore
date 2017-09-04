#include "kernel.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "streamconnector.h"
#include "versionedserializer.h"
#include "versioneddatastreamfactory.h"



using namespace Ilwis;
using namespace Stream;

std::map<StreamerKey, CreateStreamIO, LessStreamKey> VersionedDataStreamFactory::_dataStreamers;

bool LessStreamKey::operator()(const StreamerKey &val1, const StreamerKey &val2) const
{
    if ( hasType(val1._type,val2._type) && val1._version <= val2._version)
        return false;

    return val1._type < val2._type && val1._version <= val2._version;
}


VersionedDataStreamFactory::VersionedDataStreamFactory() : AbstractFactory("VersionedDataStreamFactory","ilwis")
{
}

VersionedSerializer *VersionedDataStreamFactory::create(const QString &version,IlwisTypes tp, QDataStream& stream)
{
    if ( version == "")
        return 0;

    StreamerKey key(version, tp)    ;

    auto iter =  _dataStreamers.find(key);
    if ( iter != _dataStreamers.end())
        return ((*iter).second)(stream);
    return 0;
}

void VersionedDataStreamFactory::addCreator(const StreamerKey &key, CreateStreamIO streamer)
{
   auto iter =  _dataStreamers.find(key);
   if ( iter == _dataStreamers.end())
       _dataStreamers[key] = streamer;
}



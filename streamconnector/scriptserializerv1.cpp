#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "operationmetadata.h"
#include "script.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "scriptserializerV1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ScriptSerializerV1::create(QDataStream& stream)
{
    return new ScriptSerializerV1(stream);
}

ScriptSerializerV1::ScriptSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool ScriptSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Script *script = static_cast<Script *>(obj);
    _stream << script->text();
    return true;
}

bool ScriptSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    QString txt;
    _stream >> txt;
   Script *script = static_cast<Script *>(obj);
   script->text(txt);


    return true;
}


#include "kernel.h"
#include "version.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "operationmetadataserializerv1.h"
#include "operationmetadata.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *OperationMetadataSerializerV1::create(QDataStream& stream)
{
    return new OperationMetadataSerializerV1(stream);
}

OperationMetadataSerializerV1::OperationMetadataSerializerV1(QDataStream &stream) : VersionedSerializer(stream)
{
}

bool OperationMetadataSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    //OperationMetadata *operation = static_cast<OperationMetadata *>(obj);
    if (!VersionedSerializer::store(obj, options))
        return false;

    return true;
}

bool OperationMetadataSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    //OperationMetadata *operation = static_cast<OperationMetadata *>(obj);
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    return true;
}

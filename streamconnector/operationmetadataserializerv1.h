#ifndef OPERATIONMETADATASERIALIZERV1_H
#define OPERATIONMETADATASERIALIZERV1_H

namespace Ilwis {
namespace Stream {

class OperationMetadataSerializerV1 : public VersionedSerializer
{
public:
    OperationMetadataSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // OPERATIONMETADATASERIALIZERV1_H

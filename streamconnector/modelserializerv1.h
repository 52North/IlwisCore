#ifndef MODELSERIALIZERV1_H
#define MODELSERIALIZERV1_H


namespace Ilwis {
namespace Stream {
class ModelSerializerV1 : public VersionedSerializer
{
public:
    ModelSerializerV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject *data, const IOOptions &options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // MODELSERIALIZERV1_H

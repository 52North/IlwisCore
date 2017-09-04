#ifndef STREAMELLIPSOIDDATAINTERFACEV1_H
#define STREAMELLIPSOIDDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {
class EllipsoidSerializerV1 : public VersionedSerializer
{
public:
    EllipsoidSerializerV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMELLIPSOIDDATAINTERFACEV1_H

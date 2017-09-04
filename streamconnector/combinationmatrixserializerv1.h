#ifndef COMBINATIONMATRIXSERIALIZERV1_H
#define COMBINATIONMATRIXSERIALIZERV1_H


namespace Ilwis {
namespace Stream {
class CombinationMatrixSerializerv1 : public VersionedSerializer
{
public:
    CombinationMatrixSerializerv1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject *data, const IOOptions &options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}
#endif // COMBINATIONMATRIXSERIALIZERV1_H

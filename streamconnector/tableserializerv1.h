#ifndef STREAMTABLEDATAINTERFACEV1_H
#define STREAMTABLEDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class TableSerializerV1 : public VersionedSerializer
{
public:
    TableSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
    bool loadData(IlwisObject *, const IOOptions &options = IOOptions());
    bool storeData(IlwisObject *obj, const IOOptions &options);
};
}
}

#endif // STREAMTABLEDATAINTERFACEV1_H

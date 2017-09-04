#ifndef STREAMPROJECTIONDATAINTERFACEV1_H
#define STREAMPROJECTIONDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class ProjectionSerializerV1 : public VersionedSerializer
{
public:
    ProjectionSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMPROJECTIONDATAINTERFACEV1_H

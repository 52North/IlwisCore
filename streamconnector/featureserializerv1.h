#ifndef STREAMFEATUREDATAINTERFACEV1_H
#define STREAMFEATUREDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {


class FeatureSerializerV1 : public CoverageSerializerV1
{
public:
    FeatureSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool storeData(IlwisObject *obj, const IOOptions &options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject *obj, const IOOptions &options = IOOptions());

    static Ilwis::Stream::VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMFEATUREDATAINTERFACEV1_H

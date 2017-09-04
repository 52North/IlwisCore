#ifndef STREAMGEOREFDATAINTERFACEV1_H
#define STREAMGEOREFDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class GeorefSerializerV1 : public VersionedSerializer
{
public:
    GeorefSerializerV1(QDataStream &stream);

    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMGEOREFDATAINTERFACEV1_H

#ifndef STREAMCOORDINATESYSTEMCONNECTORV1_H
#define STREAMCOORDINATESYSTEMCONNECTORV1_H

namespace Ilwis {
namespace Stream {
class CoordinateSystemSerializerV1 : public VersionedSerializer
{
public:
    CoordinateSystemSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj,const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMCOORDINATESYSTEMCONNECTOR_H

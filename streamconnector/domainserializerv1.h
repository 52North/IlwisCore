#ifndef STREAMDOMAINDATAINTERFACEV1_H
#define STREAMDOMAINDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {

class DomainSerializerV1 : public VersionedSerializer
{
public:
    DomainSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & );
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // STREAMDOMAINDATAINTERFACEV1_H

#ifndef STREAMCOVERAGEDATAINTERFACEV1_H
#define STREAMCOVERAGEDATAINTERFACEV1_H

namespace Ilwis {
namespace Stream {


class CoverageSerializerV1 : public VersionedSerializer
{
public:
    CoverageSerializerV1(QDataStream &stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
};
}
}

#endif // STREAMCOVERAGEDATAINTERFACEV1_H

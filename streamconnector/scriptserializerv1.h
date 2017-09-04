#ifndef SCRIPTSERIALIZER_H
#define SCRIPTSERIALIZER_H

namespace Ilwis {
namespace Stream {
class ScriptSerializerV1 : public VersionedSerializer
{
public:
    ScriptSerializerV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    static VersionedSerializer *create(QDataStream &stream);
};
}
}

#endif // SCRIPTSERIALIZER_H

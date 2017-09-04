#ifndef STREAMRASTERDATAINTERFACE_H
#define STREAMRASTERDATAINTERFACE_H

namespace Ilwis {
namespace Stream {



class RasterSerializerV1 : public CoverageSerializerV1
{
public:
    RasterSerializerV1(QDataStream& stream);

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions());
    bool storeData(IlwisObject *obj, const IOOptions& options = IOOptions()) override ;
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject *data, const IOOptions &options);
//    quint32 loadGridBlock(IlwisObject *data, quint32 block, QByteArray& blockdata, const RawConverter& converter, const IOOptions &options);
    static VersionedSerializer *create(QDataStream &stream);

private:


};
}
}

#endif // STREAMRASTERDATAINTERFACE_H

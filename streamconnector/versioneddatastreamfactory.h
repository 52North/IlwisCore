#ifndef VERSIONEDDATASTREAMFACTORY_H
#define VERSIONEDDATASTREAMFACTORY_H

namespace Ilwis {
namespace Stream {

typedef std::function<VersionedSerializer *(QDataStream&)> CreateStreamIO;

struct StreamerKey {
    StreamerKey(const QString& version, IlwisTypes tp) : _version(version) {
            _type = hasType(tp, itFEATURE) ? itFEATURE : tp;
    }

    QString _version;
    IlwisTypes _type;
};

struct LessStreamKey
{
  bool operator()(const StreamerKey& val1, const StreamerKey& val2 ) const;
};

class VersionedDataStreamFactory : public AbstractFactory
{
public:
    VersionedDataStreamFactory();

    static VersionedSerializer* create(const QString& version, IlwisTypes tp, QDataStream &stream);
    static void addCreator(const StreamerKey& key, CreateStreamIO streamer);

private:
    static std::map<StreamerKey, CreateStreamIO, LessStreamKey> _dataStreamers;
};

inline bool operator==(const StreamerKey& key1, const StreamerKey& key2){
    return key1._version == key2._version && hasType(key1._type, key2._type);
}
}
}

#endif // VERSIONEDDATASTREAMFACTORY_H

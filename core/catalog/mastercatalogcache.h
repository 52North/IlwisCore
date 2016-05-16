#ifndef MASTERCATALOGCACHE_H
#define MASTERCATALOGCACHE_H
#include <unordered_map>

namespace Ilwis {

class KERNELSHARED_EXPORT MasterCatalogCache
{
public:
    MasterCatalogCache();

    void store() const;
    void load();

    std::vector<Resource> find(const QUrl &url, Time modifiedtime);


private:
    std::unordered_map<uint, std::vector<Resource>> _hashes;
    const quint64 _MAGIC = 11223344556677;

    void deleteCache(QDataStream &stream);
};
}

#endif // MASTERCATALOGCACHE_H

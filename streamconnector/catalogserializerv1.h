#ifndef CATALOGSERIALIZER_H
#define CATALOGSERIALIZER_H

namespace Ilwis {
namespace Stream {
class CatalogserializerV1 : public VersionedSerializer
{
public:
    CatalogserializerV1(QDataStream &stream) ;

    bool store(IlwisObject *obj, const Ilwis::IOOptions &options= IOOptions());
    bool loadMetaData(IlwisObject*obj, const IOOptions & options);
    bool loadData(IlwisObject*, const IOOptions& options = IOOptions()) { return true; }
    bool loadItems(std::vector<Resource> &items);
    static VersionedSerializer *create(QDataStream &stream);
private:
    void adaptProperyResource(const QString& baseUrl, Resource& resource, const QString &propertyName) const;
    void adaptResource(const QString &baseUrl, Resource &resource) const;
    QString adaptedUrl(const QString &baseUrl, const Resource &resource, bool asRaw) const;
};
}
}

#endif // CATALOGSERIALIZER_H

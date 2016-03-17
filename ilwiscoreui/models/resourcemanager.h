#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <unordered_map>
#include <map>
class ResourceModel;
class MasterCatalogModel;

namespace Ilwis {

class Resource;
namespace UI{
class ResourceManager
{
public:
    friend std::unique_ptr<ResourceManager> &resourcemanager();

    ResourceManager();

    ResourceModel *createResourceModel(const QString& type, const Resource& resource);
    void deref(const QString &model, quint64 id);

    static std::unique_ptr<ResourceManager> _manager;

private:


    std::map<QString, ResourceModel *> _resources;

    ResourceModel *findResource(const QString &model, const Ilwis::Resource& resource) const;
    void addResource(ResourceModel *rm);
    MasterCatalogModel *_mastercatalogmodel = 0;
    QString key(const QString& type, quint64 id) const{ return QString::number(id) + type;}
};
}
}

std::unique_ptr<Ilwis::UI::ResourceManager> &resourcemanager();

#endif // RESOURCEMANAGER_H

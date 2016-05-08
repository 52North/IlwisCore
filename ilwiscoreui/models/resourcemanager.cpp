#include "kernel.h"
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "catalogmodel.h"
#include "resourcemanager.h"

using namespace Ilwis;
using namespace UI;

std::unique_ptr<ResourceManager> ResourceManager::_manager;

ResourceManager::ResourceManager()
{
   //connect(mastercatalog(), &MasterCatalog::contentChanged, this, &ResourceManager::containerChanged);
}

ResourceModel *ResourceManager::createResourceModel(const QString &type, const Resource &resource)
{
    auto *model = findResource(type, resource);
    if ( !model){
        if ( type == "resourcemodel"){
            model = new ResourceModel(resource);
        }
//        if ( type == "workspacemodel"){
//            model = new WorkSpaceModel(resource);
//        }
        if ( type == "operationmodel"){
            model = new OperationModel(resource);
        }
        addResource(model );
    }
    model->addref();
    return model;
}

void ResourceManager::deref(const QString& type, quint64 id)
{
    auto iter = _resources.find(key(type,id));
    if ( iter != _resources.end()){
        (*iter).second->deref();
        if ((*iter).second->ref() == 0){
            auto *model = (*iter).second;
            _resources.erase(iter);
            if ( model->modelType() == "resourcemodel") // catalogtype of models are deleted by the system
                delete model;
        }
    }
}

//void ResourceManager::containerChanged(const QString &s)
//{
//    qDebug() << "detected 1" << s;
//}



std::unique_ptr<ResourceManager>& resourcemanager()
{
    if ( !ResourceManager::_manager) {
        ResourceManager::_manager.reset(new ResourceManager());
    }
    return ResourceManager::_manager;
}

ResourceModel *ResourceManager::findResource(const QString& model, const Resource &resource) const
{
    auto iter = _resources.find(key(model, resource.id()));
    if ( iter != _resources.end())
        return (*iter).second;
    return 0;

}

void ResourceManager::addResource(ResourceModel *rm)
{
    if ( !rm || !rm->item().isValid())
        return;

    ResourceModel *model = findResource(rm->modelType(),rm->item()) ;
    if ( model)
        return;
    _resources[key(rm->modelType(), rm->item().id())] = rm;


}


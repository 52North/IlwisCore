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

}

ResourceModel *ResourceManager::createResourceModel(const QString &type, const Resource &resource)
{
    if (! _mastercatalogmodel){
        QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
        if ( mastercatalog.isValid()){
            _mastercatalogmodel = mastercatalog.value<MasterCatalogModel*>();
        }
        if (!_mastercatalogmodel)
            return 0;
    }
    auto *model = findResource(type, resource);
    // TODO for the moment disabled
//    if ( !model){
//        if ( type == "resourcemodel"){
//            model = new ResourceModel(resource);
//        }
//        if ( type == "catalogmodel"){
//            model = new CatalogModel(resource);
//        }
//        if ( type == "workspacemodel"){
//            model = new WorkSpaceModel(resource);
//        }
//        if ( type == "operationmodel"){
//            model = new OperationModel(resource);
//        }
//        addResource(model );
//    }
//    if ( model == 0){
//        qDebug() << "I am null";
//    }
//    model->addref();
    return model;
}

void ResourceManager::deref(const QString& type, quint64 id)
{
    auto iter = _resources.find(key(type,id));
//    if ( iter != _resources.end()){
//        (*iter).second->deref();
//        if ((*iter).second->ref() == 0){
//            auto *model = (*iter).second;
//            _resources.erase(iter);
//            if ( model->modelType() == "resourcemodel") // catalogtype of models are deleted by the system
//                delete model;
//        }
//    }
}



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

//    ResourceModel *model = findResource(rm->modelType(),rm->item()) ;
//    if ( model)
//        return;
//    _resources[key(rm->modelType(), rm->item().id())] = rm;


}


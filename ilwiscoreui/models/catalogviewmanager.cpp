#include "kernel.h"
#include "ilwisdata.h"
#include "catalog.h"
#include "catalogmodel.h"
#include "mastercatalog.h"
#include "catalogviewmanager.h"


Ilwis::CatalogViewManager *Ilwis::CatalogViewManager::_catalogViewManager = 0;
QTimer *Ilwis::CatalogViewManager::_timer = 0;

using namespace Ilwis;




CatalogViewManager* Ilwis::catalogViewManager(QObject *parent) {
    if (CatalogViewManager::_catalogViewManager == 0) {
        CatalogViewManager::_catalogViewManager = new CatalogViewManager(parent);
        CatalogViewManager::_catalogViewManager->connect(mastercatalog(), &MasterCatalog::contentChanged, CatalogViewManager::_catalogViewManager, &CatalogViewManager::updateCatalogViews) ;
    }
    return CatalogViewManager::_catalogViewManager;
}

CatalogViewManager::CatalogViewManager(QObject *parent) : QObject(parent)
{

}

void CatalogViewManager::updateCatalogViews(const UrlSet &containers)
{

    int typesNeeded = 0;
    for(auto url : containers){
        QString location = url.toString();
        if ( location.indexOf("ilwis://operations") == 0)
            typesNeeded |= CatalogModel::ctOPERATION;
        if ( location.indexOf("ilwis://internalcatalog") == 0)
            typesNeeded |= CatalogModel::ctDATA;
        if ( url.scheme() == "file"){
            typesNeeded |= CatalogModel::ctDATA;
        }
    }
    Locker<std::mutex> lock(_lock);
    for(auto& catalogViewItem : _catalogViews){
        int types = catalogViewItem.first->catalogType();
        int select = CatalogModel::ctBOOKMARK|CatalogModel::ctFIXED;
        if ( hasType(types,typesNeeded) && !hasType(types, select)){
            catalogViewItem.second = true;
        }
    }
}

void CatalogViewManager::doUpdate()
{
    Locker<std::mutex> lock(_lock);
    for(auto& catalogViewItem : _catalogViews){
        if ( catalogViewItem.second){
            catalogViewItem.first->refresh();
            catalogViewItem.second = false;
        }
    }
}

void CatalogViewManager::registerCatalogModel(CatalogModel *model)
{
    Locker<std::mutex> lock(_lock);
    auto iter = _catalogViews.find(model);
    if ( iter == _catalogViews.end()){
        if ( CatalogViewManager::_catalogViewManager->_timer == 0){
            CatalogViewManager::_catalogViewManager->_timer = new QTimer(CatalogViewManager::_catalogViewManager);
            CatalogViewManager::_catalogViewManager->_timer->connect(CatalogViewManager::_catalogViewManager->_timer, &QTimer::timeout, CatalogViewManager::_catalogViewManager, &CatalogViewManager::doUpdate);
            CatalogViewManager::_catalogViewManager->_timer->start(1500);
        }
        _catalogViews.insert(std::pair<CatalogModel *, bool>(model, false));
    }
}

void CatalogViewManager::unRegisterCatalogModel(CatalogModel *model)
{
    Locker<std::mutex> lock(_lock);
    auto iter = _catalogViews.find(model);
    if ( iter != _catalogViews.end())
        _catalogViews.erase(iter);
}

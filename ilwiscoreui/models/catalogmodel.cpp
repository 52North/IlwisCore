#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
#include <QCoreApplication>
#include "coverage.h"
#include "representation.h"
#include "connectorinterface.h"
#include "resource.h"
#include "geometries.h"
#include "georeference.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "tranquilizer.h"
#include "layermanager.h"
#include "ilwiscontext.h"
#include "oshelper.h"
#include "resourcemanager.h"


using namespace Ilwis;
//using namespace Desktopclient;

CatalogModel::~CatalogModel()
{
    for(auto *resource : _allItems)
        resource->deref();
}


CatalogModel::CatalogModel(QObject *parent) : ResourceModel(Resource(), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    //setFilterState(true);
}

CatalogModel::CatalogModel(const Resource &res, QObject *parent) : ResourceModel(res,parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( res.url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->resource());
        setDisplayName("default");
    }else{
        _view = CatalogView(res);
        _displayName = _view.name();
    }
    _view.setFilterState(true);
}

CatalogModel::CatalogModel(quint64 id, QObject *parent) : ResourceModel(mastercatalog()->id2Resource(id), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( item().url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->resource());
        setDisplayName("default");
    }else{
        _view = CatalogView(item());
        _displayName = _view.name();
    }
    _view.setFilterState(true);
}

void CatalogModel::scanContainer()
{
    QUrl url = resource().url();
    QThread* thread = new QThread;
    CatalogWorker2* worker = new CatalogWorker2(url);
    worker->moveToThread(thread);
    thread->connect(thread, &QThread::started, worker, &CatalogWorker2::process);
    thread->connect(worker, &CatalogWorker2::finished, thread, &QThread::quit);
    thread->connect(worker, &CatalogWorker2::finished, worker, &CatalogWorker2::deleteLater);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->connect(worker, &CatalogWorker2::updateContainer, this, &CatalogModel::updateContainer);
    thread->start();
}

void CatalogModel::setView(const CatalogView &view, bool threading){
    _view = view;
    resource(view.resource());
    bool inmainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    bool useThread = threading && inmainThread;
    //  connect(mastercatalog(),&MasterCatalog::contentChanged,this, &CatalogModel::refreshContent);
    if ( useThread){
        if ( !mastercatalog()->knownCatalogContent(OSHelper::neutralizeFileName(view.resource().url().toString()))){
            scanContainer();
        }
    }else
        mastercatalog()->addContainer(view.resource().url());
    _displayName = view.resource().name();
    if ( _displayName == sUNDEF)
        _displayName = view.resource().url().toString();
}

CatalogView CatalogModel::view() const
{
    return _view;
}

bool CatalogModel::isScanned() const
{
    return _isScanned;
}

bool CatalogModel::initNode() const {
    return _initNode;
}

int CatalogModel::level() const
{
    return _level;
}

QQmlListProperty<ResourceModel> CatalogModel::resources() {

    try{
        gatherItems();

        _objectCounts.clear();
        for(auto *resource : _allItems){
            _objectCounts[resource->type()]+= 1;
        }
        _filteredItems.clear();
        if ( _view.filterCount() == 1) // only base filter
            return  QQmlListProperty<ResourceModel>(this, _allItems);
        if ( _view.isActiveFilter("spatial"))
            fillSpatialFilter();
        if ( _view.isActiveFilter("object")){
            fillObjectFilter();
        }
        if ( _view.isActiveFilter("keyword")){
            fillKeywordFilter();
        }
        if ( _view.isActiveFilter("name")){
            fillNameFilter();
        }
        return QQmlListProperty<ResourceModel>(this, _filteredItems);
    }
    catch(const ErrorObject& err){

    }
    return  QQmlListProperty<ResourceModel>();
}

QQmlListProperty<ResourceModel> CatalogModel::coverages()
{
    _coverages.clear();
    auto& itemlist = _view.filterCount() == 1 ? _allItems : _filteredItems;
    for(auto *resource : itemlist){
        if hasType(resource->type(), itCOVERAGE){
            _coverages.push_back(resource);
        }
    }
    return QQmlListProperty<ResourceModel>(this, _coverages);
}

void CatalogModel::fillObjectFilter() {

    IlwisTypes allowedObjects = _view.objectFilter();

    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    for(ResourceModel * resource : currentList){
        if(hasType(resource->type(), allowedObjects)){
            tempList.push_back(resource);
        }
    }
    _filteredItems = QList<ResourceModel *>(tempList);

}

void CatalogModel::fillNameFilter(){
    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    auto filter = _view.filter("name");
    for(ResourceModel * resource : currentList){
        if( resource->name().indexOf(filter)!= -1){
            tempList.push_back(resource);
        }
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}

void CatalogModel::fillKeywordFilter(){
    auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
    QList<ResourceModel *> tempList;
    auto filter = _view.filter("keyword");
    QStringList keys = filter.split(",");
    for(ResourceModel * resource : currentList){
        bool ok = true;
        for(const QString& key : keys){
            if ( resource->resource().hasProperty("keyword")){
                if ( resource->keywords().indexOf(key) == -1){
                    ok = false;
                    break;
                }
            }else
                ok = false;
        }
        if ( ok){
            tempList.push_back(resource);
        }
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}


void CatalogModel::fillSpatialFilter()
{

    try{
        std::vector<double> bounds;
        QList<ResourceModel *> tempList;
        QString filter = _view.filter("spatial");
        if ( filter != ""){
            QStringList parts = filter.split(" ");
            if ( parts.size() == 4){
                bounds.push_back(parts[0].toDouble());
                bounds.push_back(parts[1].toDouble());
                bounds.push_back(parts[2].toDouble());
                bounds.push_back(parts[3].toDouble());
            }
        }
        auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
        for(ResourceModel * resource : currentList){
            if ( hasType(resource->type(), itCOVERAGE)){
                if ( bounds.size() == 4 ){
                    if ( resource->resource().hasProperty("latlonenvelope")){
                        QString envelope = resource->resource()["latlonenvelope"].toString();
                        QStringList parts = envelope.split(" ");
                        if ( parts.size() == 6){
                            if ( parts[0].toDouble() >= bounds[0] &&
                                 parts[3].toDouble() <= bounds[2] &&
                                 parts[1].toDouble() >= bounds[1] &&
                                 parts[4].toDouble() <= bounds[3])
                                tempList.push_back(resource);
                        }else if ( parts.size() == 4){
                            if ( parts[0].toDouble() >= bounds[0] &&
                                 parts[2].toDouble() <= bounds[2] &&
                                 parts[1].toDouble() >= bounds[1] &&
                                 parts[3].toDouble() <= bounds[3])
                                tempList.push_back(resource);
                        }
                    }
                }else
                    tempList.push_back(resource);
            }
        }
        _filteredItems = QList<ResourceModel *>(tempList);

    }
    catch(const ErrorObject& err){

    }
}

void CatalogModel::makeParent(QObject *obj)
{
    setParent(obj);
    if ( obj == 0)
        deleteLater();
}

void CatalogModel::filterChanged(const QString& typeIndication, bool state){
    _view.filterChanged(typeIndication, state);
    emit contentChanged();
}

void CatalogModel::filter(const QString &name, const QString &filter)
{
    _view.filter(name, filter);
    emit contentChanged();
    emit coveragesChanged();
}

void CatalogModel::addActiveFilter(const QString &filterName)
{
    _view.addActiveFilter(filterName);
    emit contentChanged();
    emit coveragesChanged();
}

void CatalogModel::removeActiveFilter(const QString &filterName)
{
    if ( _view.removeActiveFilter(filterName)){
        emit contentChanged();
        emit coveragesChanged();
    }
}

void CatalogModel::refresh(bool yesno)
{
    _refresh = yesno;
}

QStringList CatalogModel::objectCounts()
{
    try{
        gatherItems();

        _objectCounts.clear();
        for(auto *resource : _allItems){
            _objectCounts[resource->type()]+= 1;
        }
        QStringList counts;
        for(auto item : _objectCounts)    {
            QString txt = Ilwis::TypeHelper::type2HumanReadable(item.first) + "|" + QString::number(item.second);
            counts.push_back(txt);
        }
        return counts;
    }
    catch(const Ilwis::ErrorObject& err){
    }

    return QStringList();
}

void CatalogModel::refresh()
{
    _refresh = true;
    _allItems.clear();

    emit contentChanged();
    emit coveragesChanged();
}

bool CatalogModel::canBeAnimated() const
{
    bool canBeAnimated = false;
    if ( resource().hasProperty("canbeanimated")){
        canBeAnimated = resource()["canbeanimated"].toBool();
    }
    return canBeAnimated;
}


void CatalogModel::gatherItems() {
    if ( _allItems.isEmpty() || _refresh) {
        if ( !_view.isValid())
            return;

        _view.prepare();

        _allItems.clear();
        _refresh = false;

        std::vector<Resource> items = _view.items();

        for(const Resource& resource : items){
            _allItems.push_back( resourcemanager()->createResourceModel("resourcemodel", resource));
        }
        if ( _view.hasParent()){
            _allItems.push_front(resourcemanager()->createResourceModel("resourcemodel",Resource(_view.resource().url().toString() + "/..", itCATALOG)));
        }
    }
}

void CatalogModel::refreshContent(const QUrl &url)
{
    _refresh = false;
    for(auto *resource : _allItems){
        if ( resource->url() == url.toString())    {
            _refresh = true;
            break;
        }
    }
    if ( _refresh){
        emit contentChanged();
        emit coveragesChanged();
    }
}

void CatalogModel::updateContainer()
{
    _refresh = true;
    emit contentChanged();
    emit objectCountsChanged();
}

QString CatalogModel::modelType() const
{
    return "catalogmodel";
}
//-------------------------------------------------
CatalogWorker2::CatalogWorker2(const QUrl& url) : _container(url)
{
}

void CatalogWorker2::process(){
    try {
        mastercatalog()->addContainer(_container);
        emit updateContainer();
        emit finished();
    } catch(const ErrorObject& ){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }

    emit finished();
}






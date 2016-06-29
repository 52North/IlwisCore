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
#include "uicontextmodel.h"
#include "mastercatalogmodel.h"
#include "catalogviewmanager.h"
#include "oshelper.h"



using namespace Ilwis;
//using namespace Desktopclient;

CatalogModel::~CatalogModel()
{
    catalogViewManager()->unRegisterCatalogModel(this);
}


CatalogModel::CatalogModel() : ResourceModel(Resource(), 0)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
}

CatalogModel::CatalogModel(const Resource &res, int tp, QObject *parent) : ResourceModel(res,parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( tp != CatalogModel::ctDONTCARE){
        resourceRef().addProperty("catalogtype",tp);
    }else{
        if ( res.hasProperty("catalogtype")){
           resourceRef().addProperty("catalogtype",res["catalogtype"].toInt());
        }else{
            resourceRef().addProperty("catalogtype", CatalogModel::getCatalogType(res));
        }
    }
    if ( res.url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->resource());
        setDisplayName("default");
    }else{
        _view = CatalogView(res);
    }
    _view.setFilterState(true);
    catalogViewManager()->registerCatalogModel(this);
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
    }
    _view.setFilterState(true);

}

void CatalogModel::scanContainer(bool threading)
{
    bool inmainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    bool useThread = threading && inmainThread;
    if ( useThread){
        if ( !mastercatalog()->knownCatalogContent(OSHelper::neutralizeFileName(resource().url().toString()))){
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
    }else
        mastercatalog()->addContainer(resource().url());

}

void CatalogModel::setView(const CatalogView &view){
    _view = view;
}

CatalogView &CatalogModel::viewRef()
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
    auto filter = _view.filter("name").toString();
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
    for(ResourceModel * resource : currentList){
        if ( _view.keywordFilter(resource->resource()))
            tempList.push_back(resource);
    }
    _filteredItems = QList<ResourceModel *>(tempList);
}


void CatalogModel::fillSpatialFilter()
{
    try{
        QList<ResourceModel *> tempList;
        auto &currentList = _filteredItems.size() > 0 ? _filteredItems : _allItems;
        for(ResourceModel * resource : currentList){
            if ( _view.envelopeFilter(resource->resource()))
                tempList.push_back(resource);

        }
        _filteredItems = QList<ResourceModel *>(tempList);

    }
    catch(const ErrorObject& err){

    }
}

void CatalogModel::makeParent(QObject *obj)
{
    if ( obj == 0){
        catalogViewManager()->unRegisterCatalogModel(this);
        //deleteLater();
    }else
        setParent(obj);
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
    // dont do anything when
    if ( !_refresh || !_view.isValid()) {
            return;
    }

    _allItems.clear();
    _refresh = false;

    std::vector<Resource> items = _view.items();

    bool hasParent = true;
    QUrl previousContainer;
    for(const Resource& resource : items){
        _allItems.push_back( new ResourceModel(resource, this));
        hasParent &= (previousContainer.isValid() ? resource.container() == previousContainer : true);
        previousContainer = resource.container();
        if ( previousContainer.toString() == "ilwis://")
            hasParent = false;
    }
    if ( hasParent)
        _allItems.push_front(new ResourceModel(Resource(previousContainer.toString() + "/..", itCATALOG), this));

}

MasterCatalogModel *CatalogModel::getMasterCatalogModel()
{
    QVariant mastercatalog = uicontext()->rootContext()->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel*>();
        return mcmodel;
    }
    return 0;
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

int CatalogModel::catalogType() const
{
    if (resource().hasProperty("catalogtype")){
        int tp = resource()["catalogtype"].toInt();
        return tp;
    }
    return CatalogModel::ctUNKNOWN;
}
void CatalogModel::catalogType(int tp)
{
    resource()["catalogtype"] = tp;
}

CatalogModel::CatalogType CatalogModel::locationTypePart(const Resource& resource){
    QString scheme = resource.url().scheme();
    if ( scheme == "file")
        return CatalogModel::ctLOCAL;
    if ( scheme == "ilwis")
        return CatalogModel::ctINTERNAL;
    return CatalogModel::ctREMOTE;
}

int CatalogModel::getCatalogType(const Resource& res, int predefineds){
    int bits =  predefineds|
            locationTypePart(res)|
            (res.url().toString() == "ilwis://operations" ? CatalogModel::ctOPERATION : CatalogModel::ctDATA);
    if ( res.url().scheme() == "ilwis"){
        if ( res.url().toString() == "ilwis://internalcatalog")
            bits |= CatalogModel::ctMUTABLE;
        else
            bits |= CatalogModel::ctFIXED;
    }
    return bits;
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






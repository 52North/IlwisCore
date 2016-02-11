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


using namespace Ilwis;
//using namespace Desktopclient;

CatalogModel::~CatalogModel()
{

}

CatalogModel::CatalogModel(QObject *parent) : ResourceModel(Resource(), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
}

CatalogModel::CatalogModel(const Resource &res, QObject *parent) : ResourceModel(res,parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( res.url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->source());
        setDisplayName("default");
    }else{
        _view = CatalogView(res);
        _displayName = _view.name();
    }
   // connect(mastercatalog(),&MasterCatalog::contentChanged, this, &CatalogModel::refreshContent);
}

CatalogModel::CatalogModel(quint64 id, QObject *parent) : ResourceModel(mastercatalog()->id2Resource(id), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( item().url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->source());
        setDisplayName("default");
    }else{
        _view = CatalogView(item());
        _displayName = _view.name();
    }
  //  connect(mastercatalog(),&MasterCatalog::contentChanged, this, &CatalogModel::refreshContent);
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
        for(auto *resource : _currentItems){
            _objectCounts[resource->type()]+= 1;
        }

        if ( _spatialFilter != "")
            return mapItems() ;

        return  QQmlListProperty<ResourceModel>(this, _currentItems);
    }
    catch(const ErrorObject& err){

    }
    return  QQmlListProperty<ResourceModel>();
}

QQmlListProperty<ResourceModel> CatalogModel::mapItems()
{

    try{
        gatherItems();

        return  QQmlListProperty<ResourceModel>(this, _coverageItems);
    }
    catch(const ErrorObject& err){

    }
    return  QQmlListProperty<ResourceModel>();
}

void CatalogModel::makeParent(QObject *obj)
{
    setParent(obj);
    if ( obj == 0)
        deleteLater();
}

void CatalogModel::filterChanged(const QString& typeIndication, bool state){
    QString objectType = typeIndication;
    bool exclusive = objectType.indexOf("|exclusive") != -1;
    if ( exclusive)
        objectType = objectType.split("|")[0];
    if ( objectType == "all" || exclusive){
        _filterState["rastercoverage"] = exclusive ? false : state;
        _filterState["featurecoverage"] = exclusive ? false :state;
        _filterState["table"] = exclusive ? false :state;
        _filterState["coordinatesystem"] = exclusive ? false :state;
        _filterState["georeference"] = exclusive ? false :state;
        _filterState["domain"] = exclusive ? false :state;
        _filterState["representation"] = exclusive ? false :state;
        _filterState["projection"] = exclusive ? false :state;
        _filterState["ellipsoid"] = exclusive ? false :state;
    }else
        _filterState[objectType] = state;

    QString filterString;
    if ( exclusive){
        if ( state)
            filterString = QString("type") + "& '" + objectType.toLower() + "' !=0";
    } else {
        for(auto iter : _filterState){
            if ( !iter.second){
                if ( filterString != "")
                    filterString += " and ";
                filterString += QString("type") + "& '" + iter.first + "' =0";
            }
        }
    }
    filter(filterString);
}

void CatalogModel::filter(const QString &filterString)
{
    if ( _view.filter() == filterString)
        return;

    _refresh = true;
    _view.filter(filterString);
    emit contentChanged();
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
        for(auto *resource : _currentItems){
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
    _currentItems.clear();
    _coverageItems.clear();
    emit contentChanged();
}

void CatalogModel::nameFilter(const QString &filter)
{
    if ( _nameFilter == filter)
        return;

    _nameFilter = filter;
    _currentItems.clear();
    _coverageItems.clear();
    emit contentChanged();
}

bool CatalogModel::canBeAnimated() const
{
    bool canBeAnimated = false;
    if ( resource().hasProperty("canbeanimated")){
        canBeAnimated = resource()["canbeanimated"].toBool();
    }
    return canBeAnimated;
}

QString CatalogModel::nameFilter() const
{
    return _nameFilter;
}

QString CatalogModel::keyFilter() const
{
    return _keyFilter;
}

void CatalogModel::keyFilter(const QString &keyf)
{
    _keyFilter = keyf;
}

QString CatalogModel::spatialFilter() const
{
     return _spatialFilter;

}

void CatalogModel::spatialFilter(const QString &filter)
{
    _refresh = true;
    _spatialFilter = filter;
    emit contentChanged();
}

void CatalogModel::gatherItems() {
    if ( _currentItems.isEmpty() || _refresh) {
        if ( !_view.isValid())
            return;

        _view.prepare();

        _currentItems.clear();
        _coverageItems.clear();
        _refresh = false;

        std::vector<Resource> items = _view.items();
        std::vector<double> bounds(4);
        if ( _spatialFilter != ""){
            QStringList parts = _spatialFilter.split(" ");
            if ( parts.size() == 4){
                bounds[0] = parts[0].toDouble();
                bounds[1] = parts[1].toDouble();
                bounds[2] = parts[2].toDouble();
                bounds[3] = parts[3].toDouble();
            }
        }
        for(const Resource& resource : items){
            if ( _nameFilter != ""){
                if ( resource.name().indexOf(_nameFilter) == -1){
                    if ( resource["longname"].toString().indexOf(_nameFilter) == -1)
                        continue;
                }
            }
            _currentItems.push_back(new ResourceModel(resource, this));
            if ( hasType(resource.ilwisType(), itCOVERAGE)){
                if ( _spatialFilter != ""){
                    if ( resource.hasProperty("latlonenvelope")){
                        QString envelope = resource["latlonenvelope"].toString();
                        QStringList parts = envelope.split(" ");
                        if ( parts.size() == 6){
                            if ( parts[0].toDouble() >= bounds[0] &&
                                 parts[3].toDouble() <= bounds[2] &&
                                 parts[1].toDouble() >= bounds[1] &&
                                 parts[4].toDouble() <= bounds[3])
                                _coverageItems.push_back(new ResourceModel(resource, this));
                        }else if ( parts.size() == 4){
                            if ( parts[0].toDouble() >= bounds[0] &&
                                 parts[2].toDouble() <= bounds[2] &&
                                 parts[1].toDouble() >= bounds[1] &&
                                 parts[3].toDouble() <= bounds[3])
                                _coverageItems.push_back(new ResourceModel(resource, this));
                         }
                    }
                }else
                    _coverageItems.push_back(new ResourceModel(resource, this));
            }

        }
        if ( _view.hasParent()){
            _currentItems.push_front(new ResourceModel(Resource(_view.resource().url().toString() + "/..", itCATALOG), this));
        }
    }
}

void CatalogModel::refreshContent(const QUrl &url)
{
    _refresh = false;
    for(auto *resource : _currentItems){
        if ( resource->url() == url.toString())    {
            _refresh = true;
            break;
        }
    }
    if ( _refresh)
        emit contentChanged();
}

void CatalogModel::updateContainer()
{
    _refresh = true;
    emit contentChanged();
    emit objectCountsChanged();
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






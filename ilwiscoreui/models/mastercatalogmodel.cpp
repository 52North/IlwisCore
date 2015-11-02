#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonArray>
#include <QQmlContext>
#include <QSqlQuery>
#include <QQuickItem>
#include <QApplication>
#include <qtconcurrentmap.h>
#include "kernel.h"
#include "ilwisdata.h"
#include "errorobject.h"
#include "resourcemodel.h"
#include "catalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "ilwiscontext.h"
#include "ilwisconfiguration.h"
#include "tranquilizer.h"
#include "desktoptranquilizer.h"
#include "oshelper.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "ilwisobjectfactory.h"
#include "uicontextmodel.h"
#include "catalogfiltermodel.h"
#include "operationcatalogmodel.h"
#include "oshelper.h"
#include "mastercatalogmodel.h"

using namespace Ilwis;
//using namespace Desktopclient;

const int LEFTVIEW=0;
const int RIGHTVIEW=1;

MasterCatalogModel::MasterCatalogModel()
{
}

CatalogModel *MasterCatalogModel::addBookmark(const QString& label, const QUrl& location, const QString& descr, const QString& query, bool threading)
{
    Resource res(location, itCATALOGVIEW ) ;
    if ( label != "")
        res.name(label,false);
    QStringList lst;
    lst << location.toString();
    res.addProperty("locations", lst);
    res.addProperty("type", location.scheme() == "file" ? "file" : "remote");
    res.addProperty("filter","");
    res.setDescription(descr);
    CatalogView cview(res);
    cview.filter(query);
    auto cm = new CatalogModel(this);
    cm->setView(cview, threading);
    return cm;
}

MasterCatalogModel::MasterCatalogModel(QQmlContext *qmlcontext) :  _qmlcontext(qmlcontext)
{
    TranquilizerFactory *factory = kernel()->factory<TranquilizerFactory>("ilwis::tranquilizerfactory");
    factory->registerTranquilizerType(rmDESKTOP, Ilwis::Geodrawer::DesktopTranquilizer::create);

    _bookmarks.push_back(addBookmark(TR("Internal Catalog"),
               QUrl("ilwis://internalcatalog"),
               TR("All objects that are memory-based only and don't have a representation in a permanent storage"),
               "",false));
     _bookmarks.push_back(addBookmark(TR("System Catalog"),
               QUrl("ilwis://system"),
               TR("Default objects that are always available in ilwis"),
               "type<>" + QString::number(itGEODETICDATUM),false));
     _bookmarks.push_back(addBookmark(TR("Operations"),
               QUrl("ilwis://operations"),
               TR("All operations available in Ilwis"),
               "type=" + QString::number(itSINGLEOPERATION) + " or type=" + QString::number(itWORKFLOW), false));

     addDefaultFilters();
     scanBookmarks();
}

void MasterCatalogModel::addDefaultFilters(){
    //QString filter = QString("type=%1");
    QString filter = QString("type&%1!=0");
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itUNKNOWN)),"",""));
    _defaultFilters.append(new CatalogFilterModel(this,"","-- System Catalog -----------------------------",""));
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://system/domains","Domains"));
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://system/representations","Representations"));
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://system/projections","Projections"));
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://system/ellipsoids","Ellipsoids"));
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://internalcatalog","Internal Catalog"));
    _defaultFilters.append(new CatalogFilterModel(this,"","-- Most recently used --------------------------",""));

    _defaultFilters.append(new CatalogFilterModel(this,"","-- Master Catalog-------------------------------",""));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itRASTER)),"Master Catalog Rasters","raster20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itTABLE)),"Master Catalog Tables","table20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itDOMAIN)),"Master Catalog Domains","domain20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itCOORDSYSTEM)),"Master Catalog Coordinate systems","csy20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itGEOREF)),"Master Catalog Georeferences","grf20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itREPRESENTATION)),"Master Catalog Representations","rpr20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itILWISOBJECT)),"Master Catalog","all20.png"));
}

QString MasterCatalogModel::determineIdList(int dataCount, int operationCount, const QString& basekey)
{
    QString ids;
    for(int i = 0; i < operationCount; ++i){
        QString key = basekey + "/operation-" + QString::number(i);
        QString name = Ilwis::ilwisconfig(key + "/url",QString(""));
        if ( name != sUNDEF){
            quint64 id = Ilwis::mastercatalog()->name2id(name,itOPERATIONMETADATA);
            if ( id == i64UNDEF)
                continue;
            if ( ids != "")
                ids += "|";
            ids += QString::number(id);
        }
    }
    for(int i = 0; i < dataCount; ++i){
        QString key = basekey + "/data-" + QString::number(i);
        QString url = Ilwis::ilwisconfig(key + "/url",QString(""));
        quint64 type = Ilwis::ilwisconfig(key + "/type",quint64(i64UNDEF));
        if ( url != sUNDEF){
            quint64 id = Ilwis::mastercatalog()->name2id(url,type);
            if ( id == i64UNDEF){
                const IlwisObjectFactory *factory = kernel()->factory<IlwisObjectFactory>("IlwisObjectFactory",{url,type});

                if ( factory){
                    auto resources = factory->loadResource(url, type);
                    if ( resources.size() > 0){
                        id = resources[0].id();
                    }
                }

            }
            if ( ids != "")
                ids += "|";
            ids += QString::number(id);
        }
    }
    return ids;
}

void MasterCatalogModel::loadWorkSpaces(const QString workspaceList){
    QStringList parts = workspaceList.split("|");
    for(const QString& workspaceName : parts){
        QString basekey = "users/" + Ilwis::context()->currentUser() + "/workspace-" + workspaceName;
        addWorkSpace(workspaceName);
        WorkSpaceModel *wmodel = _workspaces.back();
        int operationCount = Ilwis::ilwisconfig(basekey + "/operation-count",0);
        int dataCount = Ilwis::ilwisconfig(basekey + "/data-count",0);
        QString ids = determineIdList(dataCount, operationCount, basekey);
        wmodel->addItems(ids);
    }

}

QList<std::pair<CatalogModel *, Ilwis::CatalogView> > MasterCatalogModel::startBackgroundScans(const std::vector<Ilwis::Resource>& catalogResources)
{
    QList<std::pair<CatalogModel *, CatalogView>> models;
    for(Resource resource : catalogResources){
        CatalogView view(resource);
        CatalogModel *cm = new CatalogModel(this);
        models.push_back(std::pair<CatalogModel *, CatalogView>(cm, view));
    }

    QThread* thread = new QThread;
    CatalogWorker* worker = new CatalogWorker(models);
    worker->moveToThread(thread);
    thread->connect(thread, &QThread::started, worker, &CatalogWorker::process);
    thread->connect(worker, &CatalogWorker::finished, thread, &QThread::quit);
    thread->connect(worker, &CatalogWorker::finished, worker, &CatalogWorker::deleteLater);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->connect(worker, &CatalogWorker::updateBookmarks, this, &MasterCatalogModel::updateBookmarks);
    thread->start();

    return models;
}

void MasterCatalogModel::setDefaultView()
{
    QString wcUrl = context()->workingCatalog()->source().url().toString();
    if ( wcUrl.indexOf("ilwis://internalcatalog/workspaces") == 0){
        for(auto workspace : _workspaces){
            if ( wcUrl == workspace->url()){
                _workspaces[0]->setView(workspace->view());
                _workspaces[0]->setDisplayName("default");
                break;
            }
        }
    }else{
        for(auto bookmark : _bookmarks){
            if ( OSHelper::neutralizeFileName(wcUrl) == OSHelper::neutralizeFileName(bookmark->url())){
                _workspaces[0]->setView(bookmark->view());
                _workspaces[0]->setDisplayName("default");
            }
        }
    }
}

void MasterCatalogModel::scanBookmarks()
{
    QString ids = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/available-catalog-ids",QString(sUNDEF));
    if ( ids != sUNDEF)
        _bookmarkids = ids.split("|");
    QUrl urlWorkingCatalog = context()->workingCatalog()->source().url();
    _currentUrl = urlWorkingCatalog.toString();
    int count = 3;
    std::vector<Resource> catalogResources;
    for(auto id : _bookmarkids){
        QString query = QString("users/" + Ilwis::context()->currentUser() + "/data-catalog-%1").arg(id);
        QString label = ilwisconfig(query + "/label", QString(""));
        QUrl location(ilwisconfig(query + "/url-0", QString("")));
        if ( location.isValid()) {
            QString descr = ilwisconfig(query + "/description", QString(""));
            Resource res(location, itCATALOGVIEW ) ;
            if ( label != "")
                res.name(label,false);
            QStringList lst;
            lst << location.toString();
            res.addProperty("locations", lst);
            res.addProperty("type", location.scheme() == "file" ? "file" : "remote");
            res.addProperty("filter","");
            res.setDescription(descr);

            if ( OSHelper::neutralizeFileName(urlWorkingCatalog.toString()) == OSHelper::neutralizeFileName(location.toString())){
                CatalogView cview(res);
                CatalogModel *model = 0;
                if ( location.toString().indexOf("ilwis://operations") == 0){
                    model = new OperationCatalogModel(this);
                }else
                    model = new CatalogModel(this);
                _bookmarks.push_back(model);
                _bookmarks.back()->setView(cview);

            }else{
                catalogResources.push_back(res);
            }
            ++count;
        }
    }
    count = 0;
    for(auto bookmark : _bookmarks){
        if ( bookmark->resource().url() == urlWorkingCatalog){
            _selectedBookmarkIndex = count;
        }
        ++count;
    }

    _workspaces.push_back(new WorkSpaceModel("default", this));
    QString availableWorkspaces = Ilwis::ilwisconfig("users/" + Ilwis::context()->currentUser() + "/workspaces",QString(""));
    if ( availableWorkspaces != ""){
        loadWorkSpaces(availableWorkspaces);
    }
    uicontext()->setCurrentWorkSpace(_workspaces.back());

    // workspaces[0] is the default workspace; its view can't be correct(yet) as it is created before any views
    // are determined. as the default has a starting view that must point to the start working catalog it will
    // search for the workspace that represents this and copy its view
    setDefaultView();

    QList<std::pair<CatalogModel *, CatalogView>> models = startBackgroundScans(catalogResources);

    for(auto iter = models.begin(); iter != models.end(); ++iter){
        _bookmarks.push_back((*iter).first);
    }

    if ( _currentUrl == ""){
        if ( _bookmarks.size() > 3){
            _selectedBookmarkIndex = 3;
            _currentUrl = _bookmarks[_selectedBookmarkIndex]->url();
        }else {
            QString loc = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
            _currentUrl = "file:///" + loc;
        }
    }
}

QQmlListProperty<CatalogFilterModel> MasterCatalogModel::defaultFilters()
{
    return QQmlListProperty<CatalogFilterModel>(this,_defaultFilters);
}

QQmlListProperty<CatalogModel> MasterCatalogModel::bookmarked()
{
   return  QQmlListProperty<CatalogModel>(this, _bookmarks);
}

QQmlListProperty<WorkSpaceModel> MasterCatalogModel::workspaces()
{
    return QQmlListProperty<WorkSpaceModel>(this, _workspaces);
}

int MasterCatalogModel::activeTab() const
{
    return _activeTab;
}

void MasterCatalogModel::setActiveTab(int value)
{
    _activeTab = value;
}

QString MasterCatalogModel::getName(const QString &id)
{
    bool ok;
    quint64 objid = id.toULongLong(&ok);
    if ( ok){
        Resource res = mastercatalog()->id2Resource(objid);
        if ( res.isValid())
            return res.name();
    }
    return "";
}

QString MasterCatalogModel::getUrl(const QString &id)
{
    bool ok;
    quint64 objid = id.toULongLong(&ok);
    if ( ok){
        Resource res = mastercatalog()->id2Resource(objid);
        if ( res.isValid())
            return res.url().toString();
    }
    return "";
}

QString MasterCatalogModel::id2type(const QString &id) const
{
    bool ok;
    quint64 objid = id.toULongLong(&ok);
    if ( ok){
        IlwisTypes tp = mastercatalog()->id2type(objid);
        return TypeHelper::type2name(tp);
    }
    return "";
}
void MasterCatalogModel::longAction()
{

    QThread *thr = new QThread;

    worker *w = new worker;

    w->moveToThread(thr);
    thr->connect(thr, &QThread::started, w, &worker::process);
    thr->start();
}

std::vector<Resource> MasterCatalogModel::select(const QString &filter)
{
    return mastercatalog()->select(filter);

}

void MasterCatalogModel::updateBookmarks()
{
    emit bookmarksChanged();
}

quint32 MasterCatalogModel::selectedBookmark(const QString& url)
{
    for(int  i=0; i < _bookmarks.size(); ++i){
        if ( _bookmarks[i]->url() == url){
            _currentUrl = url;
            return i;
        }
    }
    return iUNDEF;
}

void MasterCatalogModel::setSelectedBookmark(quint32 index)
{
    if ( index < _bookmarks.size()){
        mastercatalog()->addContainer(QUrl(_bookmarks[index]->url()));
        context()->setWorkingCatalog(ICatalog(_bookmarks[index]->url()));
        _currentUrl = _bookmarks[index]->url();

        if ( index > 1) // system and internal, not normal bookmarks
            _selectedBookmarkIndex = index - 2;
        else
            _selectedBookmarkIndex = 2;

    }
}

QQmlListProperty<IlwisObjectModel> MasterCatalogModel::selectedData()
{
    return  QQmlListProperty<IlwisObjectModel>(this, _selectedObjects);
}

IlwisObjectModel *MasterCatalogModel::id2object(const QString &objectid, QQuickItem *parent)
{
    bool ok;
    Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
    if (!ok)
        return 0;

    IlwisObjectModel *ioModel = new IlwisObjectModel(resource, parent);
    if ( ioModel->isValid()){
        return ioModel;
    }
    return 0;
}

void MasterCatalogModel::setSelectedObjects(const QString &objects)
{
    try {
        if ( objects == ""){
            _selectedObjects.clear();
            emit selectionChanged();
            return;
        }
        QStringList parts = objects.split("|");
        _selectedObjects.clear();
        kernel()->issues()->silent(true);
        for(auto objectid : parts){
            bool ok;
            Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
            if (!ok)
                continue;

            IlwisObjectModel *ioModel = new IlwisObjectModel(resource, this);
            if ( ioModel->isValid()){
                _selectedObjects.append(ioModel);
                emit selectionChanged();
            }else
                delete ioModel;
        }
        kernel()->issues()->silent(false);
    }catch(const ErrorObject& ){
    }catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    kernel()->issues()->silent(false);
}

bool MasterCatalogModel::hasSelectedObjects() const
{
    return _selectedObjects.size() != 0;
}

CatalogModel *MasterCatalogModel::newCatalog(const QString &inpath, const QString& filter)
{
    if ( inpath == "" || inpath == sUNDEF )
        return 0;

    CatalogView cview;
    QUrl location(inpath);
    _currentUrl = inpath;
    if ( inpath.indexOf("ilwis://internalcatalog/workspaces") == 0){
        for(auto workspace : _workspaces){
            if ( workspace->url() == inpath){
                emit currentCatalogChanged();
                auto wsm = new WorkSpaceModel(*workspace);
                return wsm;
            }
        }
    }else {
        bool canBeAnimated = false;
        Resource collection = mastercatalog()->name2Resource(inpath,itRASTER);
        if ( collection.isValid()){
            canBeAnimated = hasType(collection.extendedType(), itCATALOG)        ;
        }
        Resource res(location, itCATALOGVIEW ) ;
        QStringList lst;
        lst << ((inpath.indexOf("http://") == 0) ? res.container().toString() : inpath);
        res.addProperty("locations", lst);
        QString scheme = location.scheme();
        if ( scheme == "file")
            res.addProperty("type", "file");
        if ( scheme == "http" || scheme == "https")
            res.addProperty("type", "remote");
        if ( scheme == "ilwis")
            res.addProperty("type", "internal");
        if ( filter != "container=ilwis://mastercatalog")
            res.addProperty("filter",filter);
        res.addProperty("canbeanimated",canBeAnimated);
        cview = CatalogView(res);
        CatalogModel *model = 0;
        if ( inpath.indexOf("ilwis://operations") == 0){
            model = new OperationCatalogModel(this);
        }else
            model = new CatalogModel(this);
        model->setView(cview, true);
        emit currentCatalogChanged();
        return model;


    }
    return 0;
}

QString MasterCatalogModel::getDrive(quint32 index){
    QFileInfoList drives = QDir::drives();
    if ( index < drives.size()){
        return drives[index].filePath();
    }
    return "";
}

QStringList MasterCatalogModel::driveList() const{
#ifdef Q_OS_WIN
     QFileInfoList drives = QDir::drives();
     QStringList drivenames;
     for(auto item : drives){
        drivenames.append(item.filePath());
     }
     return drivenames;
#else
    return QStringList();
#endif
}

void MasterCatalogModel::addBookmark(const QString& path){
    if ( path == "")
        return;

    for(auto cat : _bookmarks){
        if ( cat->url() == path)
            return;
    }
    _bookmarks.push_back(addBookmark(path,path,"",""));


    int catid = _bookmarkids.size() == 0 ? 0 : _bookmarkids.last().toInt() + 1;
    QString newid = QString::number(catid);
    _selectedBookmarkIndex = _bookmarks.size();
    _bookmarkids.push_back(newid);
    _currentUrl = path;

    QString basekey = "users/" + Ilwis::context()->currentUser() + "/data-catalog-" + newid;

    context()->configurationRef().addValue(basekey + "/url-0", path);
    context()->configurationRef().addValue(basekey + "/nr-of-urls", "1");
    int index = path.lastIndexOf("/");
    QString label = path.mid(index + 1);
    context()->configurationRef().addValue(basekey + "/label", label);
    QString availableid = _bookmarkids.join("|");
    context()->configurationRef().addValue("users/" + Ilwis::context()->currentUser() + "/available-catalog-ids", availableid);


}

void MasterCatalogModel::addWorkSpace(const QString &name)
{
    if ( name == "" || name.toLower() == "default")
        return;

    for(auto ws : _workspaces){
        if ( ws->name() == name)
            return;
    }
    WorkSpaceModel *wmodel = new WorkSpaceModel(name, this);
    QString path = "ilwis://internalcatalog/workspaces/" + name;
    Resource resource(path,itWORKSPACE);
    CatalogView view(resource);
    wmodel->setView(view);
    _workspaces.push_back(wmodel);
    uicontext()->setCurrentWorkSpace(wmodel);
    emit workspacesChanged();
}

void MasterCatalogModel::removeWorkSpace(const QString &name)
{
    //TODO
}

WorkSpaceModel *MasterCatalogModel::workspace(const QString &name)
{
    for(auto ws : _workspaces){
        if ( ws->name() == name)
            return ws;
    }
    return 0;
}


void MasterCatalogModel::deleteBookmark(quint32 index){
    if ( index < _bookmarks.size() && index > 1)  { // can not delete internal , system catalog, operations
        _bookmarks.erase(_bookmarks.begin() + index);
        QString key = "users/" + Ilwis::context()->currentUser() + "/data-catalog-" + _bookmarkids[index - 3];
        context()->configurationRef().eraseChildren(key);
        _bookmarkids.erase(_bookmarkids.begin() + index - 3);
        if ( _bookmarkids.size() > 0)
            _selectedBookmarkIndex = 2;

        QString availableid = _bookmarkids.join("|");
        context()->configurationRef().addValue("users/" + Ilwis::context()->currentUser() + "/available-catalog-ids", availableid);
    }
}

void MasterCatalogModel::setCatalogMetadata(const QString& displayName, const QString& description){
    if ( _selectedBookmarkIndex < 3)
        return;

    CatalogModel *model = _bookmarks[_selectedBookmarkIndex];
    if ( model ){
        model->setDisplayName(displayName);
        model->resourceRef().setDescription(description); 
        QString key = "users/user-0/data-catalog-" + _bookmarkids[_selectedBookmarkIndex - 3];
        context()->configurationRef().putValue(key + "/label", displayName);
        context()->configurationRef().putValue(key + "/description", description);
        emit bookmarksChanged();
    }
}

QStringList MasterCatalogModel::select(const QString &filter, const QString& property)
{
    std::vector<Resource> resources = mastercatalog()->select(filter);
    QStringList resourceList;
    for (const auto& resource : resources){
        if (resource.isValid()){
            if ( property == "name")
                resourceList.append(QString::number(resource.id()) + "|" + resource.name());
        }
    }
    return resourceList;
}

ResourceModel* MasterCatalogModel::id2Resource(const QString &objectid)
{
    bool ok;
    Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
    if (ok && resource.isValid()){
        ResourceModel *model = new ResourceModel(resource,this);
        return model;
    }
    qDebug() << " wrong id used";
    return 0;
}

QStringList MasterCatalogModel::knownCatalogs(bool fileonly)
{
    QStringList folders;
    std::vector<Resource> catalogs = mastercatalog()->select("type=" + QString::number(itCATALOG));
    for(const auto& resource : catalogs){
        if ( resource.url().scheme() != "file" && fileonly == false)
            folders.append(resource.url().toString());
        else if ( fileonly && resource.url().scheme() == "file"){
            folders.append(resource.url().toLocalFile());
        }
    }
    return folders;
}

void MasterCatalogModel::setWorkingCatalog(const QString &path)
{
    if ( path != ""){
        context()->setWorkingCatalog(ICatalog(path));
        _currentUrl = path;
    }
}



void MasterCatalogModel::refreshWorkingCatalog()
{
    auto items = context()->workingCatalog()->items();
    mastercatalog()->removeItems(items);

    QThread* thread = new QThread;
    CatalogWorker3* worker = new CatalogWorker3(_currentCatalog->resource());
    worker->moveToThread(thread);
    thread->connect(thread, &QThread::started, worker, &CatalogWorker3::process);
    thread->connect(worker, &CatalogWorker3::finished, thread, &QThread::quit);
    thread->connect(worker, &CatalogWorker3::finished, worker, &CatalogWorker3::deleteLater);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->connect(worker, &CatalogWorker3::updateContainer, _currentCatalog, &CatalogModel::updateContainer);
    thread->start();

}

int MasterCatalogModel::activeSplit() const
{
    return _activeSplit;
}

void MasterCatalogModel::setActiveSplit(int index)
{
    if ( index == 0 || index == 1){
        _activeSplit = index;
    }
}

QString MasterCatalogModel::currentUrl() const
{
    return _currentUrl;
}

void MasterCatalogModel::setCurrentUrl(const QString &url)
{
    _currentUrl = url;
}

CatalogModel *MasterCatalogModel::currentCatalog() const
{
    return _currentCatalog;
}

void MasterCatalogModel::setCurrentCatalog(CatalogModel *cat)
{
    if ( cat->url() == Catalog::DEFAULT_WORKSPACE){
        _currentCatalog = new CatalogModel(Ilwis::Resource(context()->workingCatalog()->source().url().toString(), itCATALOG), this);
    } else
        _currentCatalog = cat;
}



WorkSpaceModel *MasterCatalogModel::workspace(quint64 id)
{
    for(auto workspace : _workspaces){
        if ( workspace->id().toULongLong() == id)
            return workspace;
    }
    return 0;
}

quint32 MasterCatalogModel::workspaceIndex(const QString &name)
{
    quint32 index = 0;
    for(auto workspace : _workspaces){
        if ( name == workspace->name())
            return index;
        ++index;
    }
    return iUNDEF;
}



void MasterCatalogModel::updateCatalog(const QUrl &url)
{

}

QString MasterCatalogModel::selectedIds() const
{
    QString selected;
    for(auto obj : _selectedObjects ){
        if ( selected != "")
            selected += "|";
        selected += obj->id();

    }
    return selected;
}

void MasterCatalogModel::deleteObject(const QString &id)
{
    bool ok;
    quint64 objid = id.toULongLong(&ok);
    Resource resource = mastercatalog()->id2Resource(objid);
    IIlwisObject obj;
    obj.prepare(resource);
    if ( !obj.isValid())
        return;
    obj->remove();
}
//--------------------
CatalogWorker::CatalogWorker(QList<std::pair<CatalogModel *, CatalogView> > &models) : _models(models)
{
}

CatalogWorker::~CatalogWorker(){
}

void CatalogWorker::process(){
    try {
        for(auto iter = _models.begin(); iter != _models.end(); ++iter){
            (*iter).first->setView((*iter).second);
            emit updateBookmarks();
        }
        if (!uicontext()->abort()){
            //calculatelatLonEnvelopes();
            emit finished();
        }
    } catch(const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }

    emit finished();
}

void CatalogWorker::calcLatLon(const ICoordinateSystem& csyWgs84,Ilwis::Resource& resource, std::vector<Resource>& updatedResources){
    try{
        if ( !resource.hasProperty("latlonenvelope")){
            ICoverage cov(resource);
            if ( cov.isValid()){
                if ( cov->coordinateSystem()->isLatLon()){
                    QString envelope = cov->envelope().toString();
                    resource.addProperty("latlonenvelope",envelope);
                }else {
                    Envelope env = cov->envelope();
                    if ( env.isNull() || !env.isValid())
                        return;
                    Envelope llEnvelope = csyWgs84->convertEnvelope(cov->coordinateSystem(), env);
                    if ( llEnvelope.isNull() || !llEnvelope.isValid())
                        return;

                    resource.addProperty("latlonenvelope",llEnvelope.toString());
                }
                updatedResources.push_back(resource);
            }
        }
    } catch(const ErrorObject&){
    } catch( std::exception& ){
    }
}

void CatalogWorker::calculatelatLonEnvelopes(){
    kernel()->issues()->silent(true);
    QString query = QString("(type & %1) != 0").arg(QString::number(itCOVERAGE));
    std::vector<Resource> resources =mastercatalog()->select(query);
    UPTranquilizer trq(Tranquilizer::create(context()->runMode()));
    trq->prepare("LatLon Envelopes","calculating latlon envelopes",resources.size());
    ICoordinateSystem csyWgs84("code=epsg:4326");
    std::vector<Resource> updatedResources;
    for(Resource& resource : resources){
        calcLatLon(csyWgs84, resource, updatedResources);
        if(!trq->update(1))
            return;

    }
    kernel()->issues()->silent(false);
    mastercatalog()->updateItems(updatedResources);

}

//---------------------
void worker::process(){
    Ilwis::UPTranquilizer trq(Tranquilizer::create(context()->runMode()));
    trq->prepare("test operation","bbb", 10000);
    for(int i = 0; i < 10000; ++i){
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
            trq->update(100);
    }
}

//--------------------------
void CatalogWorker3::process()
{
    try{
        ICatalog catalog(OSHelper::neutralizeFileName(_resource.url().toString()));
        if ( !catalog.isValid()){
            return ;
        }
        catalog->scan();
        emit updateContainer();
        emit finished();

    } catch(const ErrorObject& ){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }

    emit finished();
}

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
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#ifdef Q_OS_LINUX
#include <QProcess>

#include <QString>
#endif
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
#include "resourcemanager.h"
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
    res.addProperty("filter",query);
    res.setDescription(descr);
    auto cm = new CatalogModel(res, CatalogModel::getCatalogType(res, CatalogModel::ctBOOKMARK));
    cm->scanContainer(threading);
    return cm;
}

MasterCatalogModel::MasterCatalogModel(QQmlContext *qmlcontext) :  _qmlcontext(qmlcontext)
{
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
    _defaultFilters.append(new CatalogFilterModel(this,"ilwis://internalcatalog","Temporary Catalog"));
    _defaultFilters.append(new CatalogFilterModel(this,"","-- Most recently used --------------------------",""));

    _defaultFilters.append(new CatalogFilterModel(this,"","-- Master Catalog-------------------------------",""));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itRASTER)),"Master Catalog Rasters","raster20.png"));
    _defaultFilters.append(new CatalogFilterModel(this,filter.arg(QString::number(itCOVERAGE)),"Master Catalog Coverages","mapview.png"));
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

QList<CatalogModel *> MasterCatalogModel::startBackgroundScans(const std::vector<Ilwis::Resource>& catalogResources)
{
    QList<CatalogModel *> models;
    for(Resource resource : catalogResources){
        CatalogModel *cm = new CatalogModel(resource, CatalogModel::getCatalogType(resource));
        models.push_back(cm);
    }

    QThread* thread = new QThread;
    CatalogWorker* worker = new CatalogWorker(catalogResources, true);
    thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
    worker->moveToThread(thread);
    thread->connect(thread, &QThread::started, worker, &CatalogWorker::process);
    thread->connect(worker, &CatalogWorker::finished, thread, &QThread::quit);
    thread->connect(worker, &CatalogWorker::finished, worker, &CatalogWorker::deleteLater);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->connect(worker, &CatalogWorker::updateBookmarks, this, &MasterCatalogModel::updateBookmarks);
    thread->connect(worker, &CatalogWorker::finished, this, &MasterCatalogModel::initFinished);
    thread->start();

    return models;
}

void MasterCatalogModel::initFinished() {
    bool isFinished = Ilwis::context()->initializationFinished();
    Ilwis::context()->initializationFinished(true);
    if (!isFinished)
        _currentCatalog->refresh();
}

void MasterCatalogModel::setDefaultView()
{
    QString wcUrl = context()->workingCatalog()->resource().url().toString();
    if ( wcUrl.indexOf("ilwis://internalcatalog/workspaces") == 0){
        for(auto workspace : _workspaces){
            if ( wcUrl == workspace->url()){
                _workspaces[0]->setView(workspace->viewRef());
                _workspaces[0]->setDisplayName("default");
                break;
            }
        }
    }else{
        for(auto bookmark : _bookmarks){
            if ( OSHelper::neutralizeFileName(wcUrl) == OSHelper::neutralizeFileName(bookmark->url())){
                _workspaces[0]->setView(bookmark->viewRef());
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
    QUrl urlWorkingCatalog = context()->workingCatalog()->resource().url();
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
            res.addProperty("filter","container='" + location.toString() +"'");
            res.setDescription(descr);

            if ( OSHelper::neutralizeFileName(urlWorkingCatalog.toString()) == OSHelper::neutralizeFileName(location.toString())){
                CatalogModel *model = 0;
                if ( location.toString().indexOf("ilwis://operations") == 0){
                    model = new OperationCatalogModel(this);
                }else
                    model = new CatalogModel(res, CatalogModel::getCatalogType(res, CatalogModel::ctBOOKMARK));
                model->scanContainer(false);
                _bookmarks.push_back(model);

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

    QList<CatalogModel *> models = startBackgroundScans(catalogResources);

    for(auto iter = models.begin(); iter != models.end(); ++iter){
        _bookmarks.push_back(*iter);
    }

    if ( _currentUrl == ""){
        if ( _bookmarks.size() > 3){
            _selectedBookmarkIndex = 3;
            _currentUrl = _bookmarks[_selectedBookmarkIndex]->url();
        }else {
            QString loc = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
            _currentUrl = OSHelper::createFileUrlFromParts(loc, "");
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

bool MasterCatalogModel::isCompatible(const QString &objUrl1, const QString &objUrl2, const QString &type)
{
    if ( type == "georeference"){
        if ( objUrl1 == objUrl2)
            return true;

        IGeoReference grf1(objUrl1, itGEOREF, {"mustexist", true});
        IGeoReference grf2(objUrl2, itGEOREF, {"mustexist", true});
        if ( grf1.isValid() && grf2.isValid()){
            return grf1->isCompatible(grf2);
        }
    }
    return false;
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

IlwisObjectModel *MasterCatalogModel::id2object(const QString &objectid, QObject *parent)
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
        auto clearList = [&](){
            for(IlwisObjectModel *model : _selectedObjects){
                model->setParent(0);
                model->deleteLater();
            }
            _selectedObjects.clear();
        };

        if ( objects == ""){
            clearList();
            emit selectionChanged();
            return;
        }
        QStringList parts = objects.split("|");
        clearList();
        kernel()->issues()->silent(true);
        for(auto objectid : parts){
            bool ok;
            Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
            if (!ok)
                continue;

            IlwisObjectModel *ioModel = new IlwisObjectModel(resource, this);
            _selectedObjects.append(ioModel);
            emit selectionChanged();
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
        if ( filter != "container=ilwis://mastercatalog")
            res.addProperty("filter",filter);
        res.addProperty("canbeanimated",canBeAnimated);

        CatalogModel *model = 0;
        if ( inpath.indexOf("ilwis://operations") == 0){
            res.addProperty("filter", "(type=" + QString::number(itSINGLEOPERATION) + " or type=" + QString::number(itWORKFLOW) + ")");
            model = new OperationCatalogModel(res, this);
        }else
            model = new CatalogModel(res, CatalogModel::getCatalogType(res, CatalogModel::ctMUTABLE));
        if ( _currentCatalog == 0){
            QString num = context()->configurationRef()("users/" + Ilwis::context()->currentUser() + "/filter-count",QString("0"));
            int n = num.toInt();
            for(int i = 0; i < n; ++i){
                QString basekey = "users/" + Ilwis::context()->currentUser() + "/filters-" + QString::number(i);
                QString name = context()->configurationRef()(basekey + "/filter-name", QString(""));
                QString newfilter = context()->configurationRef()(basekey + "/filter-defintion", QString(""));
                model->filter(name,newfilter);
            }
        }

        model->scanContainer(true);
        emit currentCatalogChanged();
        return model;


    }
    return 0;
}

// TODO insure that the drive "index" is coherent
QString MasterCatalogModel::getDrive(quint32 index){
    QStringList drives = MasterCatalogModel::driveList();

    if ( index < drives.size()){
        return drives[index];
    }
    return "";
}

QStringList MasterCatalogModel::driveList() const{
    QStringList drivenames;
#ifdef Q_OS_WIN
     QFileInfoList drives = QDir::drives();
     for(auto item : drives){
        drivenames.append(item.filePath());
     }
#endif
#ifdef Q_OS_LINUX
     QProcess process;
     process.start("lsblk", QStringList() << "-o" << "MOUNTPOINT");

     if (process.waitForFinished()) {
         QByteArray result = process.readAll();
         if (result.length() > 0) {
             QStringList mountpoints = QString(result).split('\n', QString::SplitBehavior::SkipEmptyParts);
             QStringList unwantedStrings("MOUNTPOINT");
             unwantedStrings.append("[SWAP]");

             for (QString mountp: mountpoints) {
                 if (!unwantedStrings.contains(mountp))
                     drivenames.append(mountp);
             }
         }
     }
#endif
     return drivenames;
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
    if ( property == "epsg" ){ // special case because its a very big list
        QString query = QString("select itemid,name,code from mastercatalog where  %2 ").arg(filter);
        InternalDatabaseConnection results(query);

        std::map<quint32, QString> orderlist;
        while( results.next()) {
            QString epsgCode = results.value(2).toString().mid(5);
            int code = epsgCode.toInt();
            //qDebug() << code << results.value(2).toString();
            QString str = QString("%1|%2|%3").arg(results.value(0).toString()).arg(results.value(1).toString()).arg(code,5,10,QChar('0'));
            orderlist[code] = str ;
        }

        QStringList items;
        for(auto& item : orderlist)        {
            items.append(item.second);
        }
        return items;
    }else {
        std::vector<Resource> resources = mastercatalog()->select(filter);
        QStringList resourceList;
        for (const auto& resource : resources){
            if (resource.isValid()){
                QString result = QString::number(resource.id());
                if ( property == "name")
                    result += "|" + resource.name();
                resourceList.append(result);
            }
        }
        return resourceList;
    }
}

ResourceModel* MasterCatalogModel::id2Resource(const QString &objectid, QObject *parent)
{
    bool ok;
    Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
    if (ok && resource.isValid()){
        ResourceModel *model =new ResourceModel(resource, parent);
        return model;
    }else { // might be an anonymous object
        auto obj = mastercatalog()->get(objectid.toULongLong(&ok));
        if ( obj){
            Resource res = obj->resource();
            ResourceModel *model =new ResourceModel(resource, parent);
            return model;
        }
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
         mastercatalog()->addContainer(QUrl(path));
    }
}



void MasterCatalogModel::refreshCatalog(const QString& path)
{
    auto items = context()->workingCatalog()->items();
    mastercatalog()->removeItems(items);

    QThread* thread = new QThread;
    CatalogWorker3* worker = new CatalogWorker3(path);
    thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
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
    if (!_rootObject){
        QQmlApplicationEngine *engine = dynamic_cast<QQmlApplicationEngine *>(_qmlcontext->engine());
        if ( engine){
            QObject *top = engine->rootObjects().value(0);
            QObject *topLevel = top->findChild<QObject *>("mainwindow__mainui");
            if ( topLevel){
                _rootObject = topLevel;
            }
        }
    }
    if ( cat->url() == Catalog::DEFAULT_WORKSPACE){
        Resource res = Ilwis::Resource(context()->workingCatalog()->resource().url().toString(), itCATALOG);
        _currentCatalog = new CatalogModel(res, CatalogModel::getCatalogType(res), this);
    } else{
        _currentCatalog = cat;
        if ( _rootObject){
            QObject *obj = _rootObject->findChild<QObject *>("main_ui_catalogform");
            if ( obj && cat->url().indexOf("file://") == 0){
                obj->setProperty("currentFolder", cat->url());
            }
        }
    }
    ICatalog catalog(cat->url());
    if ( catalog.isValid()){
        context()->setWorkingCatalog(catalog);
        mastercatalog()->addContainer(cat->url());
    }
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


void MasterCatalogModel::prepare()
{
    TranquilizerFactory *factory = kernel()->factory<TranquilizerFactory>("ilwis::tranquilizerfactory");
    factory->registerTranquilizerType(rmDESKTOP, Ilwis::Geodrawer::DesktopTranquilizer::create);

    _bookmarks.push_back(addBookmark(TR("Temporary Catalog"),
               QUrl("ilwis://internalcatalog"),
               TR("All objects that are memory-based only and don't have a representation in a permanent storage"),
               "type=" + QString::number(itANY) + " and container='ilwis://internalcatalog'",false));
     _bookmarks.push_back(addBookmark(TR("System Catalog"),
               QUrl("ilwis://system"),
               TR("Default objects that are always available in ilwis"),
               "type<>" + QString::number(itGEODETICDATUM),false));
     _bookmarks.push_back(addBookmark(TR("Operations"),
               QUrl("ilwis://operations"),
               TR("All operations available in Ilwis"),
               "(type=" + QString::number(itSINGLEOPERATION) + " or type=" + QString::number(itWORKFLOW) + ")", false));

     addDefaultFilters();
     scanBookmarks();

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

bool MasterCatalogModel::exists(const QString &url, const QString &objecttype)
{
    IlwisTypes tp = IlwisObject:: name2Type(objecttype)   ;
    if ( tp ==itUNKNOWN)
        return false;
    quint64 id = mastercatalog()->name2id(url,tp);
   return  id != i64UNDEF;
}

//--------------------
CatalogWorker::CatalogWorker(const std::vector<Ilwis::Resource>& resources, bool initial) : _catalogs(resources), _initial(false)
{
}

CatalogWorker::~CatalogWorker(){
}

void CatalogWorker::process(){
    try {
        for(auto& resource : _catalogs){
            scanContainer(resource.url(), _catalogs.size() != 1);
            if ( !_initial){
                QString query = QString("(type & %1) != 0 and container='%2'").arg(QString::number(itCOVERAGE)).
                        arg(OSHelper::neutralizeFileName(resource.url().toString()));
                calculatelatLonEnvelopes(query, resource.name());
            }

        }
//        QString query = QString("(type & %1) != 0 and container='%2'").arg(QString::number(itCOVERAGE)).
//                arg(OSHelper::neutralizeFileName(context()->workingCatalog()->resource().url().toString()));
//        calculatelatLonEnvelopes(query,"working catalog");
        if ( _initial){
            qDebug() << " also latlon ";
             QString query = QString("(type & %1) != 0").arg(QString::number(itCOVERAGE));
             calculatelatLonEnvelopes(query, "mastercatalog");
        }
        emit updateCatalog();
        emit updateBookmarks();
       emit finished();

    } catch(const ErrorObject& err){

    } catch ( const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }

    emit finished();
}

void CatalogWorker::scanContainer(const QUrl& url, bool threading)
{
    bool inmainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    bool useThread = threading && !inmainThread;
    if ( useThread){
         if ( !mastercatalog()->knownCatalogContent(OSHelper::neutralizeFileName(url.toString()))){
             QThread* thread = new QThread;
             CatalogWorker2* worker = new CatalogWorker2(url);
             thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
             worker->moveToThread(thread);
             thread->connect(thread, &QThread::started, worker, &CatalogWorker2::process);
             thread->connect(worker, &CatalogWorker2::finished, thread, &QThread::quit);
             thread->connect(worker, &CatalogWorker2::finished, worker, &CatalogWorker2::deleteLater);
             thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
             //thread->connect(worker, &CatalogWorker2::updateContainer, this, &CatalogModel::updateContainer);
             thread->start();
         }
     }else
         mastercatalog()->addContainer(url);

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
//--------------------------
void CatalogWorker3::process()
{
    try{
        ICatalog catalog(OSHelper::neutralizeFileName(_path));
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

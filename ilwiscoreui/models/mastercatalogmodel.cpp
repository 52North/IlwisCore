#include <QCoreApplication>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonArray>
#include <QQmlContext>
#include <QSqlQuery>
#include <QQuickItem>
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
#include "mastercatalogmodel.h"

using namespace Ilwis;
//using namespace Desktopclient;

const int LEFTVIEW=0;
const int RIGHTVIEW=1;

MasterCatalogModel::MasterCatalogModel()
{

}

MasterCatalogModel::MasterCatalogModel(QQmlContext *qmlcontext) :  _qmlcontext(qmlcontext)
{
    _splitCatalogs.resize(2);
    int nrOfcatalogs = ilwisconfig("users/user-0/nr-of-data-catalogs",0);
    for(int cat = 0; cat < nrOfcatalogs; ++cat){
        QString query = QString("users/user-0/data-catalog-%1").arg(cat);
        QString label = ilwisconfig(query + "/label", QString(""));
        QUrl location(ilwisconfig(query + "/url-0", QString("")));
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
        CatalogView cview(res);
        _bookmarks.push_back(new CatalogModel(cview,0,this));
    }
    if ( _bookmarks.size() > 0)
        _splitCatalogs[LEFTVIEW].push_back(_bookmarks[0]);

}

QQmlListProperty<CatalogModel> MasterCatalogModel::bookmarked()
{
   return  QQmlListProperty<CatalogModel>(this, _bookmarks);
}

CatalogModel* MasterCatalogModel::selectedCatalog()
{
    if ( _splitCatalogs[_activeSplit].size() > 0)
        return _splitCatalogs[_activeSplit][LEFTVIEW];
    return 0;
}

CatalogModel* MasterCatalogModel::selectedCatalog() const
{
    if ( _splitCatalogs[_activeSplit].size() > 0)
        return _splitCatalogs[_activeSplit][LEFTVIEW];
    return 0;
}

std::vector<Resource> MasterCatalogModel::select(const QString &filter)
{
    return mastercatalog()->select(filter);

}

quint32 MasterCatalogModel::selectedIndex() const
{
    return 0;
}

quint32 MasterCatalogModel::selectedBookmark() const
{
    for(int  i=0; i < _bookmarks.size(); ++i){
        if ( _bookmarks[i]->url() == selectedCatalog()->url()){
            return i;
        }
    }
    return iUNDEF;
}

void MasterCatalogModel::setSelectedBookmark(quint32 index)
{
    if ( index < _bookmarks.size()){
        _splitCatalogs[_activeSplit][0] = _bookmarks[index];
        mastercatalog()->addContainer(QUrl(_bookmarks[index]->url()));
        context()->setWorkingCatalog(ICatalog(_bookmarks[index]->url()));
         emit emitResourcesChanged();
    }
}

void MasterCatalogModel::pushToFront(int index)
{
    CatalogModel *cat = _splitCatalogs[_activeSplit][index] ;
    _splitCatalogs[_activeSplit].erase(_splitCatalogs[_activeSplit].begin() + index);
    _splitCatalogs[_activeSplit].push_front(cat);
}

void MasterCatalogModel::setSelectedIndex(const QString& path)
{
    if ( path == "")
        return;

    if ( _root){
        _currentList = _activeSplit == LEFTVIEW ? leftResources() : rightResources();
        mastercatalog()->addContainer(QUrl(path));
        context()->setWorkingCatalog(ICatalog(path));
        int index = 0;
        for(const auto cat : _splitCatalogs[_activeSplit]){
            if ( cat->url() == path){
                break;    
            }
            ++index;
        }
        if ( index < _splitCatalogs[_activeSplit].size()){
            pushToFront(index);
        }
        emit emitResourcesChanged();
    }
}

QMLResourceList MasterCatalogModel::leftResources(){
    if ( _splitCatalogs[LEFTVIEW].size() > 0)
        return _splitCatalogs[LEFTVIEW][0]->resources();

    return QQmlListProperty<ResourceModel>();
}

QMLResourceList MasterCatalogModel::rightResources(){
    if ( _splitCatalogs[RIGHTVIEW].size() > 0)
        return _splitCatalogs[RIGHTVIEW][0]->resources();

    return QQmlListProperty<ResourceModel>();
}

void MasterCatalogModel::root(QObject *obj)
{
    _root = obj;
}

QString MasterCatalogModel::currentUrl() const
{
    if(_splitCatalogs[_activeSplit].size() > 0)
        return _splitCatalogs[_activeSplit][LEFTVIEW]->url();
    return "";
}

void MasterCatalogModel::updateCatalog(const QUrl &url)
{
    setSelectedIndex(url.toString());
}

void MasterCatalogModel::emitResourcesChanged()
{
    if (_activeSplit == LEFTVIEW)
        emit leftResourcesChanged();
    else
        emit rightResourcesChanged();
}

void MasterCatalogModel::addCatalog(const QString &inpath, int splitIndex)
{
    if ( inpath == "" || inpath == sUNDEF)
        return;

    _activeSplit = splitIndex;
    QString newcatalogurl = changeCatalog(inpath);
    if ( newcatalogurl == "")
        return ;
    QUrl location(newcatalogurl);
    Resource res(location, itCATALOGVIEW ) ;
    QStringList lst;
    lst << ((newcatalogurl.indexOf("http://") == 0) ? res.container().toString() : newcatalogurl);
    res.addProperty("locations", lst);
    res.addProperty("type", location.scheme() == "file" ? "file" : "remote");
    res.addProperty("filter","");
    CatalogView cview(res);
    _splitCatalogs[splitIndex].push_front(new CatalogModel(cview,0, this));
    emitResourcesChanged();

}

QString MasterCatalogModel::changeCatalog(const QString &inpath){
    if ( inpath == "" || inpath == sUNDEF)
        return "";
    CatalogModel *model = selectedCatalog();
    if  ( model && model->url() == inpath)
        return "";

    QString path = inpath;
    int index = inpath.indexOf("/..");
    if ( index != -1){
        path = path.left(path.size() - 3);
        index = path.lastIndexOf("/");
        if ( index == -1)
            return "";
        path = path.left(index);
    }
    //we are not sure here if it is a filepath or an url; we need an url though
    QFileInfo inf(path);
    // qfileinfo only works with filepaths no urls, so if it exists it is a path
    QString url =  inf.exists() ? QUrl::fromLocalFile(path).toString() : path;

    for(int i =0; i < _splitCatalogs[_activeSplit].size(); ++i){
        CatalogModel *cat = _splitCatalogs[_activeSplit][i];
        if ( cat->url() == url){
            pushToFront(i);
            emit emitResourcesChanged();
            return cat->url();
        }
    }
    return url;
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
    _bookmarks.push_back(selectedCatalog());
}


void MasterCatalogModel::deleteBookmark(quint32 index){
    if ( index < _bookmarks.size())  {
        _bookmarks.erase(_bookmarks.begin() + index)    ;
    }
}

void MasterCatalogModel::setCatalogMetadata(const QString& displayName, const QString& description){
    CatalogModel *model = selectedCatalog();
    if ( model){
        model->setDisplayName(displayName);
        model->resourceRef().setDescription(description); 
    }
}

void MasterCatalogModel::setObjectFilterCurrentCatalog(const QString &objecttype, bool state)
{
    CatalogModel *catalogModel = selectedCatalog();
    if ( catalogModel){
        catalogModel->filterChanged(objecttype, state);
        emit emitResourcesChanged();
    }
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
        emit emitResourcesChanged();
    }
}

void MasterCatalogModel::refreshWorkingCatalog()
{
    CatalogModel *catalogModel = selectedCatalog();
    if ( catalogModel) {
        auto items = context()->workingCatalog()->items();
        mastercatalog()->removeItems(items);
        context()->workingCatalog()->scan();
        catalogModel->refresh(true);
    }

    emit emitResourcesChanged();
}

void MasterCatalogModel::setIndex(const QModelIndex &index, const QVariant &value, int role)
{
    
}

QQmlListProperty<IlwisObjectModel> MasterCatalogModel::selectedData()
{
     CatalogModel *catalogModel = selectedCatalog();
     if ( catalogModel)
         return catalogModel->selectedData();
     return QQmlListProperty<IlwisObjectModel>();
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

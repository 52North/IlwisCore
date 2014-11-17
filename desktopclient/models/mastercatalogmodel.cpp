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

MasterCatalogModel::MasterCatalogModel()
{

}

MasterCatalogModel::MasterCatalogModel(QQmlContext *qmlcontext) :  _qmlcontext(qmlcontext)
{
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
        _activeCatalogs[res.url().toString()] = new CatalogModel(cview,0,this);
        _bookmarkedList.push_back(res.url().toString());
    }

}

QQmlListProperty<CatalogModel> MasterCatalogModel::bookmarked()
{
    _bookmarks.clear();
    for(auto index : _bookmarkedList)
        _bookmarks.push_back(_activeCatalogs[index]);

   return  QQmlListProperty<CatalogModel>(this, _bookmarks);

}

CatalogModel* MasterCatalogModel::selectedCatalog()
{
    if ( _activeCatalogs.size() > 0){
        if ( _currentUrl.isEmpty())
            return _activeCatalogs.begin()->second;
        return _activeCatalogs[_currentUrl];
    }
    return 0;
}

quint32 MasterCatalogModel::selectedIndex() const
{
    return 0;
}

quint32 MasterCatalogModel::selectedBookmark() const
{
    for(int  i=0; i < _bookmarkedList.size(); ++i){
        if ( _bookmarkedList[i] == _currentUrl){
            return i;
        }
    }
    return iUNDEF;
}

void MasterCatalogModel::setSelectedBookmark(quint32 index)
{
    if ( index < _bookmarkedList.size()){
        _currentUrl = _bookmarkedList[index];
        mastercatalog()->addContainer(QUrl(_currentUrl));
         emit resourcesChanged();
    }
}

void MasterCatalogModel::setSelectedIndex(const QString& path)
{
    _currentUrl = path;
    if ( _root){
        _currentList = resources();
        mastercatalog()->addContainer(QUrl(_currentUrl));
        emit resourcesChanged();
    }
}

QMLResourceList MasterCatalogModel::resources(){
    CatalogModel *catalogModel = selectedCatalog();
    if ( catalogModel)
        return catalogModel->resources();

    return QQmlListProperty<ResourceModel>();
}

void MasterCatalogModel::root(QObject *obj)
{
    _root = obj;
}

void MasterCatalogModel::updateCatalog(const QUrl &url)
{
    setSelectedIndex(url.toString());
}

void MasterCatalogModel::addCatalog(const QString &inpath)
{
    QString path = inpath;
    int index = inpath.indexOf("/..");
    if ( index != -1){
        path = path.left(path.size() - 3);
        index = path.lastIndexOf("/");
        if ( index == -1)
            return;
        path = path.left(index);
    }
    //we are not sure here if it is a filepath or an url; we need an url though
    QFileInfo inf(path);
    // qfileinfo only works with filepaths no urls, so if it exists it is a path
    QString url =  inf.exists() ? QUrl::fromLocalFile(path).toString() : path;

    for(auto item : _activeCatalogs){
        if ( item.first == url){
            _currentUrl = url;
            emit resourcesChanged();
            return;
        }

    }
    QUrl location(url);
    Resource res(location, itCATALOGVIEW ) ;
    QStringList lst;
    lst << ((url.indexOf("http://") == 0) ? res.container().toString() : url);
    res.addProperty("locations", lst);
    res.addProperty("type", location.scheme() == "file" ? "file" : "remote");
    res.addProperty("filter","");
    CatalogView cview(res);
    _activeCatalogs[url] =new CatalogModel(cview,0);
    _currentUrl = url;
    emit resourcesChanged();

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

    if ( std::find(_bookmarkedList.begin(), _bookmarkedList.end(), path) == _bookmarkedList.end()){
        _bookmarkedList.push_back(path);
    }
}


void MasterCatalogModel::deleteBookmark(quint32 index){
    if ( index < _bookmarkedList.size())  {
        _bookmarkedList.erase(_bookmarkedList.begin() + index)    ;
    }
}

void MasterCatalogModel::setCatalogMetadata(const QString& displayName, const QString& description){
    if ( std::find(_bookmarkedList.begin(), _bookmarkedList.end(), _currentUrl) != _bookmarkedList.end()){
        if ( _activeCatalogs.find(_currentUrl) != _activeCatalogs.end()){
        CatalogModel *model = _activeCatalogs[_currentUrl];
            model->setDisplayName(displayName);
            model->resourceRef().setDescription(description);
        }
    }
}

void MasterCatalogModel::setObjectFilterCurrentCatalog(const QString &objecttype, bool state)
{
    CatalogModel *catalogModel = selectedCatalog();
    if ( catalogModel){
        catalogModel->filterChanged(objecttype, state);
        emit resourcesChanged();
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
    if ( path != "")
        context()->setWorkingCatalog(ICatalog(path));
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

    emit resourcesChanged();
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


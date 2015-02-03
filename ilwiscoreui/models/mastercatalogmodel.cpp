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

CatalogModel *MasterCatalogModel::addBookmark(const QString& label, const QUrl& location, const QString& descr, const QString& query)
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
    return new CatalogModel(cview,0,this);
}

MasterCatalogModel::MasterCatalogModel(QQmlContext *qmlcontext) :  _qmlcontext(qmlcontext)
{
    _bookmarks.push_back(addBookmark(TR("Internal Catalog"),
               QUrl("ilwis://internalcatalog"),
               TR("All objects that are memory-based only and don't have a representation in a permanent storage"),
               ""));


     _bookmarks.push_back(addBookmark(TR("System Catalog"),
               QUrl("ilwis://system"),
               TR("Default objects that are always available in ilwis"),
               "type<>" + QString::number(itGEODETICDATUM)));

    QString ids = ilwisconfig("users/user-0/available-catalog-ids",QString("0"));
    _bookmarkids = ids.split("|");
    QUrl urlWorkingCatalog = context()->workingCatalog()->source().url();
    int count = 2;
    for(auto id : _bookmarkids){
        QString query = QString("users/user-0/data-catalog-%1").arg(id);
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
        if ( urlWorkingCatalog == location){
            _selectedBookmarkIndex = count;
            _currentUrl = urlWorkingCatalog.toString();
        }
        ++count;
    }
    if ( _currentUrl == ""){
        if ( _bookmarks.size() > 2){
            _selectedBookmarkIndex = 2;
            _currentUrl = _bookmarks[_selectedBookmarkIndex]->url();
        }else {
            QString loc = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
            _currentUrl = "file:///" + loc;
        }
    }


}

QQmlListProperty<CatalogModel> MasterCatalogModel::bookmarked()
{
   return  QQmlListProperty<CatalogModel>(this, _bookmarks);
}

int MasterCatalogModel::activeTab() const
{
    return _activeTab;
}

void MasterCatalogModel::setActiveTab(int value)
{
    _activeTab = value;
}

std::vector<Resource> MasterCatalogModel::select(const QString &filter)
{
    return mastercatalog()->select(filter);

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

CatalogModel *MasterCatalogModel::newCatalog(const QString &inpath)
{
    if ( inpath == "" || inpath == sUNDEF)
        return 0;

    QUrl location(inpath);
    Resource res(location, itCATALOGVIEW ) ;
    QStringList lst;
    lst << ((inpath.indexOf("http://") == 0) ? res.container().toString() : inpath);
    res.addProperty("locations", lst);
    res.addProperty("type", location.scheme() == "file" ? "file" : "remote");
    res.addProperty("filter","");
    _currentUrl = inpath;
    CatalogView cview(res);
    auto model = new CatalogModel(cview,0, this);
    return model;
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

    QString basekey = "users/user-0/data-catalog-" + newid;

    context()->configurationRef().addValue(basekey + "/url-0", path);
    context()->configurationRef().addValue(basekey + "/nr-of-urls", "1");
    int index = path.lastIndexOf("/");
    QString label = path.mid(index + 1);
    context()->configurationRef().addValue(basekey + "/label", label);
    QString availableid = _bookmarkids.join("|");
    context()->configurationRef().addValue("users/user-0/available-catalog-ids", availableid);


}


void MasterCatalogModel::deleteBookmark(quint32 index){
    if ( index < _bookmarks.size() && index > 1)  { // can not delete internal and system catalog
        _bookmarks.erase(_bookmarks.begin() + index);
        QString key = "users/user-0/data-catalog-" + _bookmarkids[index - 2];
        context()->configurationRef().eraseChildren(key);
        _bookmarkids.erase(_bookmarkids.begin() + index - 2);
        if ( _bookmarkids.size() > 0)
            _selectedBookmarkIndex = 2;

        QString availableid = _bookmarkids.join("|");
        context()->configurationRef().addValue("users/user-0/available-catalog-ids", availableid);
    }
}

void MasterCatalogModel::setCatalogMetadata(const QString& displayName, const QString& description){
    CatalogModel *model = _bookmarks[_selectedBookmarkIndex];
    if ( model ){
        model->setDisplayName(displayName);
        model->resourceRef().setDescription(description); 
        QString key = "users/user-0/data-catalog-" + _bookmarkids[_selectedBookmarkIndex];
        context()->configurationRef().putValue(key + "/label", displayName);
        context()->configurationRef().putValue(key + "/description", description);
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
        _currentUrl = path;
    }
}

void MasterCatalogModel::refreshWorkingCatalog()
{
        auto items = context()->workingCatalog()->items();
        mastercatalog()->removeItems(items);
        context()->workingCatalog()->scan();
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
    _currentCatalog = cat;
}

void MasterCatalogModel::updateCatalog(const QUrl &url)
{

}

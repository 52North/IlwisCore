#include "catalogscontroller.h"

using namespace Ilwis;
using namespace Desktopclient;

CatalogsController::CatalogsController() : _isInitial(true),_catalog1IsActive(false)
{
}

CatalogsController::~CatalogsController()
{
    qDeleteAll(pathList);
    qDeleteAll(catalogList1);
    qDeleteAll(catalogList2);
}

void CatalogsController::changeCatalogForward( QQmlContext *qmlcontext, const QString &sid)
{
    Ilwis::Resource resource;
    if ( _isInitial) {
        resource = Ilwis::mastercatalog()->id2Resource(sid.toLongLong());
    }
    else {
        long index = sid.toLong();
        ResourceModel *rm = static_cast<ResourceModel *>(catalogList1[index]);
        resource = rm->resource();
        if ( !resource.isValid())
            return;
    }

    _catalog1IsActive = !_catalog1IsActive;
    openCatalog(qmlcontext, resource);
}

void CatalogsController::changeCatalogBackward( QQmlContext *qmlcontext, const QString &sid) {
    long index = sid.toLong();
    ResourceModel *rm = static_cast<ResourceModel *>(pathList[index]);
    Ilwis::Resource resource = rm->resource();
    if ( !resource.isValid())
        return;
    int sz = pathList.size() - 1;
    for(int i = sz; i >= index; --i) {
        delete pathList.last();
        pathList.pop_back();
    }

    openCatalog(qmlcontext, resource);
}

Ilwis::Resource CatalogsController::getResource(const QString& sid) {
    bool ok;
    quint64 id = sid.toLongLong(&ok);
    if (!ok) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Item id", "resource"), Ilwis::IssueObject::itWarning);
        return Ilwis::Resource();
    }
    Ilwis::Resource resource = Ilwis::mastercatalog()->id2Resource(id);
    if (!resource.isValid()) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Item id", "resource"), Ilwis::IssueObject::itWarning);
        return Ilwis::Resource();
    }
    return resource;

}


void CatalogsController::getResourceList(const Ilwis::Resource& resource, const QString& query, QList<QObject *>& list) {
    Ilwis::ICatalog catalog;
    list.clear();
    catalog.prepare(resource.url().toString());
    std::vector<Resource> items = catalog->items();
    std::sort(items.begin(), items.end());
    foreach(const Ilwis::Resource& item , items) {
        list.append(new ResourceModel(item));
    }
}

void CatalogsController::makePathList(const Ilwis::Resource& resource) {
    qDeleteAll(pathList);
    pathList.clear();

    Ilwis::Resource res = resource;

    pathList.push_front(new ResourceModel(res));
    while(res.name() != "root") {
        quint64 id = Ilwis::mastercatalog()->url2id(res.container(), itCATALOG);
        res = Ilwis::mastercatalog()->id2Resource(id);
        pathList.push_front(new ResourceModel(res));
    }
}

void CatalogsController::openCatalog( QQmlContext *qmlcontext, const Ilwis::Resource& resource) {

   // qDeleteAll(catalogList2);
    catalogList2 = catalogList1;
    getResourceList(resource, "type='Catalog'", catalogList1);
    if ( _isInitial) {
        catalogList2 = catalogList1;
        _isInitial = false;
    }

    QList<QObject *> dataList;

    getResourceList(resource, "", dataList);

    makePathList(resource);

    //pathList.append(new ResourceModel(resource));



    //Ilwis::context()->setWorkingCatalog(cat);
    qmlcontext->setContextProperty("catalogItemModel", QVariant::fromValue(dataList));
    qmlcontext->setContextProperty("catalogsModel1", QVariant::fromValue(catalogList1));
    qmlcontext->setContextProperty("catalogsModel2", QVariant::fromValue(catalogList2));
    qmlcontext->setContextProperty("pathModel", QVariant::fromValue(pathList));
}


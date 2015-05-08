#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "identity.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "catalogview.h"
#include "catalogquery.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"

using namespace Ilwis;

CatalogView::CatalogView(QObject *parent) :
    QObject(parent)
{
}

CatalogView::CatalogView(const QUrl &loc) : QObject(){
    addLocation(loc);
    QStringList parts = loc.toString().split("/");
    name(parts.back());
    _resource = Resource("ilwis://internalcatalog/" + name(), itCATALOGVIEW);
    QStringList lst;
    lst.append(loc.toString());
    _resource["locations"] = lst;
}

CatalogView::CatalogView(const Resource &resource) : QObject(), Identity(resource.name(),resource.id(),resource.code()){
    filter(resource["filter"].toString());
    QStringList lst = resource["locations"].toStringList();
    for(auto url : lst){
        if ( url != sUNDEF)
            _locations.push_back(QUrl(url));
    }
    _resource = resource;
}

CatalogView::CatalogView(const CatalogView &cat) : QObject(),
    Identity(sUNDEF,i64UNDEF,cat.code(),cat.description()),
     _filter(cat._filter),
     _locations(cat._locations),
     _fixedItems(cat._fixedItems),
     _parent(cat._parent),
     _resource(cat.resource())
 {
     Identity::prepare(); // bit inconveniet but the id must be set. this overrules the name so we set it again
     name(cat.name());
}

void CatalogView::addLocation(const QUrl& loc){
    if ( std::find(_locations.begin(), _locations.end(), loc) == _locations.end()){
        _locations.push_back(loc);
        if ( name() != sUNDEF)
            return;

        QStringList parts = loc.toString().split("/");
        QString cid = parts.back();
        name(cid);
    }
}

std::vector<Resource> CatalogView::items() const
{
    if (!isValid())
        return std::vector<Resource>();

    std::vector<Resource> results;
    for(auto& item : _fixedItems){
        results.push_back(item.second);
    }
    QString filter = _filter;
    for(auto location : _locations) {
        std::vector<Resource> items;
        if ( location == QUrl("ilwis://system")){
            if ( filter != "")
                filter += " and ";
            filter += QString("type<>%1").arg(QString::number(itGEODETICDATUM)); // datums are not visible in the catalogs
        }
        items =  mastercatalog()->select(location, filter);
        std::copy(items.begin(), items.end(),std::back_inserter(results));
    }
    std::set<Resource> uniques(results.begin(), results.end());
    results.resize(uniques.size());
    std::copy(uniques.begin(), uniques.end(), results.begin());
    return results;

}

void CatalogView::addFixedItem(quint64 id)
{
    Resource resource = mastercatalog()->id2Resource(id);
    if ( resource.isValid()){
        _fixedItems[id] = resource;
    }
}

void CatalogView::removeFixedItem(quint64 id)
{
    auto iter = _fixedItems.find(id);
    if ( iter != _fixedItems.end())
        _fixedItems.erase(iter);
}

quint32 CatalogView::fixedItemCount() const
{
    return _fixedItems.size();
}

QString CatalogView::filter() const
{
    return _filter;
}

void CatalogView::filter(const QString &filter)
{
    CatalogQuery query;
    _filter = query.transformQuery(filter);
}

Resource CatalogView::resource() const
{
    return _resource;
}



bool CatalogView::prepare()
{
    for(auto resource : _locations)
        if(!mastercatalog()->addContainer(resource))
            return false;

    Identity::prepare();



    return true;
}

CatalogView &CatalogView::operator=(const CatalogView &view)
{
    Identity::prepare();
    name(view.name());
    setDescription(view.description());
    code(view.code());
    _filter = view._filter;
    _locations = view._locations;
    _resource = view._resource;
    _fixedItems = view._fixedItems;

    return *this;

}

QString CatalogView::type() const
{
    return "CatalogView";
}


bool CatalogView::isValid() const
{
    return _locations.size() > 0 || _fixedItems.size() > 0 ;
}

QUrl CatalogView::parentCatalogView() const
{
    return _parent;
}

void CatalogView::setParentCatalogView(const QUrl &url)
{
    _parent = url;
}
















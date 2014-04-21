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
    _filter = resource["filter"].toString();
    QStringList lst = resource["locations"].toStringList();
    for(auto url : lst){
        _locations.push_back(QUrl(url));
    }
    _resource = resource;
}

CatalogView::CatalogView(const CatalogView &cat) : QObject(),
    Identity(cat.name(),cat.id(),cat.code(),cat.description()),
    _filter(cat._filter),
    _locations(cat._locations),
    _parent(cat._parent),
    _resource(cat.resource())
{
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
    for(auto location : _locations) {
        std::vector<Resource> items =  mastercatalog()->select(location, _filter);
        std::copy(items.begin(), items.end(),std::back_inserter(results));
    }
    std::set<Resource> uniques(results.begin(), results.end());
    results.resize(uniques.size());
    std::copy(uniques.begin(), uniques.end(), results.begin());
    return results;

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
    name(view.name());
    setDescription(view.description());
    code(view.code());
    _filter = view._filter;
    _locations = view._locations;
    _resource = view._resource;

    return *this;

}

QString CatalogView::type() const
{
    return "CatalogView";
}


bool CatalogView::isValid() const
{
    return _locations.size() > 0; // && _filter != "";
}

QUrl CatalogView::parentCatalogView() const
{
    return _parent;
}

void CatalogView::setParentCatalogView(const QUrl &url)
{
    _parent = url;
}
















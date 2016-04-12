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
#include "ilwisconfiguration.h"

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
    filter("basefilter", resource["filter"].toString());
    QStringList lst = resource["locations"].toStringList();
    for(auto url : lst){
        if ( url != sUNDEF)
            _locations.push_back(QUrl(url));
    }
    _resource = resource;
}

CatalogView::CatalogView(const CatalogView &cat) : QObject(),
    Identity(sUNDEF,i64UNDEF,cat.code(),cat.description()),
     _locations(cat._locations),
     _fixedItems(cat._fixedItems),
     _parent(cat._parent),
     _resource(cat.resource())
 {
     Identity::prepare(); // bit inconveniet but the id must be set. this overrules the name so we set it again
     name(cat.name());
     _filters = cat._filters;
     _activeFilters = cat._activeFilters;
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
    auto iter = _filters.find("basefilter");
    QString filter = iter != _filters.end() ? (*iter).second._filter.toString() : "";
    for(auto location : _locations) {
        std::vector<Resource> items;
        if ( location == QUrl("ilwis://system")){
            if ( filter != "")
                filter += " and ";
            filter += QString("type<>%1").arg(QString::number(itGEODETICDATUM)); // datums are not visible in the catalogs
        }
        if ( location != MasterCatalog::MASTERCATALOG){
            items =  mastercatalog()->select(location, filter);
        }else
            items =  mastercatalog()->select(filter);

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

//void CatalogView::filter(const QString &filter)
//{
//    CatalogQuery query;
//    _filter = query.transformQuery(filter);
//}

Resource CatalogView::resource() const
{
    return _resource;
}

bool CatalogView::hasParent() const
{
    if ( _locations.size() != 1)
        return false;
    return _locations[0].scheme() == "file";
}

bool CatalogView::isActiveFilter(const QString &name) const
{
    return std::find(_activeFilters.begin(), _activeFilters.end(), name) != _activeFilters.end();
}

bool CatalogView::prepare()
{

    Identity::prepare();
    return true;
}

void CatalogView::filterChanged(const QString& typeIndication, bool state){
    IlwisTypes tp = IlwisObject::name2Type(typeIndication);
    auto iter  = _filters.find("object");
    if ( iter == _filters.end()){
        _filters["object"] = FilterItem("object",itANY);

    }
    quint64 currentTypes = _filters["object"]._filter.toULongLong();
    if ( tp != itUNKNOWN){
        if ( state)
            currentTypes |= tp;
        else
            currentTypes &= ~tp;
    }else {
        if ( state)
            currentTypes = itANY;
        else
            currentTypes = itUNKNOWN;
    }
    _filters["object"]._filter = currentTypes;

    addActiveFilter("object");
}

int CatalogView::filterCount() const
{
    return _activeFilters.size();
}

IlwisTypes CatalogView::objectFilter() const
{
    auto iter  = _filters.find("object");
    if ( iter != _filters.end())
        return (*iter).second._filter.toULongLong();
    return itANY;
//    IlwisTypes type = itANY;
//    for(auto iter : _filterState){
//        if ( !iter.second){
//            type &= ~iter.first;
//        }
//    }
//    return type;
}

void CatalogView::storeFilters() const
{
    int count = 0;
    for(auto filter : _filters) {
        QString basekey = "users/" + Ilwis::context()->currentUser() + "/filters-" + QString::number(count++);
        context()->configurationRef().addValue(basekey + "/filter-name", filter.first);
        context()->configurationRef().addValue(basekey + "/filter-defintion", filter.second._filter.toString());
    }
    context()->configurationRef().addValue("users/" + Ilwis::context()->currentUser() + "/filter-count", QString::number(count));

}

void CatalogView::filter(const QString &name, const QString &filter)
{
    if ( name == "")
        return;

    auto iter = _filters.find(name);
    if ( iter != _filters.end() && (*iter).second._filter == filter)
        return;
    if ( filter == ""){
        auto iter = _filters.find(name);
        if ( iter != _filters.end()){
            _filters.erase(name);
        }
        removeActiveFilter(name);

    }else {
        _filters[name] = FilterItem(name,filter);
        addActiveFilter(name);
    }

}

QString CatalogView::filter(const QString& name) const
{
    auto iter = _filters.find(name);
    if ( iter != _filters.end()){
        return (*iter).second._filter.toString();
    }
    return "";
}

void CatalogView::addActiveFilter(const QString &filterName)
{
    if ( filterName != "")
     _activeFilters.insert(filterName);
}

bool CatalogView::removeActiveFilter(const QString &filterName)
{
    auto iter = _activeFilters.find(filterName);
    if ( iter != _activeFilters.end()){
        _activeFilters.erase(iter);
        return true;
    }
    return false;
}

CatalogView &CatalogView::operator=(const CatalogView &view)
{
    Identity::prepare();
    name(view.name());
    setDescription(view.description());
    code(view.code());
    _filters = view._filters;
    _activeFilters = view._activeFilters;
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

void CatalogView::setFilterState(bool state)
{
    _filterState[itRASTER] = state;
    _filterState[itFEATURE] = state;
    _filterState[itTABLE] = state;
    _filterState[itCOORDSYSTEM] = state;
    _filterState[itGEOREF] = state;
    _filterState[itDOMAIN] = state;
    _filterState[itREPRESENTATION] = state;
    _filterState[itPROJECTION] = state;
    _filterState[itELLIPSOID] = state;
    _filterState[itCATALOG] = state;
}
















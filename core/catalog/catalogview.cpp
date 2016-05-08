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
#include "geometries.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "ilwisconfiguration.h"

using namespace Ilwis;

CatalogView::CatalogView(QObject *parent) :
    QObject(parent)
{
}

CatalogView::CatalogView(const Resource &resource) : QObject(){
    filter("basefilter", resource["filter"].toString());
}

CatalogView::CatalogView(const QString &baseFilter) : QObject(){
    filter("basefilter", baseFilter);
}

CatalogView::CatalogView(const CatalogView &cat) : QObject(),
     _fixedItems(cat._fixedItems)
 {
     _filters = cat._filters;
     _activeFilters = cat._activeFilters;
     _filterState = cat._filterState;
}

std::vector<Resource> CatalogView::items() const
{
    std::vector<Resource> results;
    for(auto& item : _fixedItems){
        results.push_back(item.second);
    }
    auto iter = _filters.find("basefilter");
    QString filter = iter != _filters.end() ? (*iter).second._filter.toString() : "";
    results =  mastercatalog()->select(filter);

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

bool CatalogView::isActiveFilter(const QString &name) const
{
    return std::find(_activeFilters.begin(), _activeFilters.end(), name) != _activeFilters.end();
}


void CatalogView::filterChanged(const QString& typeIndication, bool state){
    QString type = typeIndication;
    bool exclusive = false;
    if ( typeIndication.indexOf("|exclusive") != -1){
        type = typeIndication.split("|")[0];
        exclusive = true;
    }
    IlwisTypes tp = IlwisObject::name2Type(type);
    auto iter  = _filters.find("object");
    if ( iter == _filters.end()){
        _filters["object"] = FilterItem("object",itANY);

    }
    quint64 currentTypes = _filters["object"]._filter.toULongLong();
    if ( exclusive){
        if ( tp != itUNKNOWN){
            if ( state)
                currentTypes = tp;
            else
                currentTypes = itANY;
        }else {
            if ( state)
                currentTypes = itANY;
            else
                currentTypes = itUNKNOWN;
        }
    }else {
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
        QVariant storedFilter = filter;
        if ( name == "spatial"){
            QStringList parts = filter.split(" ");
            if ( parts.size() == 4){
                Envelope env(filter);
                storedFilter.setValue(env);
            }
        }

        _filters[name] = FilterItem(name,storedFilter);
        addActiveFilter(name);
    }

}

bool CatalogView::hasFilter(const QString &name) const
{
    return _filters.find(name) != _filters.end();
}

QVariant CatalogView::filter(const QString& name) const
{
    auto iter = _filters.find(name);
    if ( iter != _filters.end()){
        return (*iter).second._filter;
    }
    return QVariant();
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
    _filters = view._filters;
    _activeFilters = view._activeFilters;
    _fixedItems = view._fixedItems;
    _filterState = view._filterState;

    return *this;

}

QString CatalogView::type() const
{
    return "CatalogView";
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

bool CatalogView::envelopeFilter(const Resource resource) const
{
    if ( hasType(resource.ilwisType(), itCOVERAGE)){
         if ( hasFilter("spatial") ){
            if ( resource.hasProperty("latlonenvelope")){
                Envelope env = filter("spatial").value<Envelope>();
                QString envelope = resource["latlonenvelope"].toString();
                Envelope llEnv(envelope);
                if ( env.contains(llEnv))
                    return true;
            }
        }else
            return true; // if there is no spatail filter everything is by defintion ok
    }
    return false;
}

bool CatalogView::isValid() const
{
    return _filters.size() != 0;
}

bool CatalogView::keywordFilter(const Resource resource) const{
    if ( !hasFilter("keyword"))
        return true;
    if (!resource.hasProperty("keyword"))
        return false;
    QString keywords = resource["keyword"].toString();
    auto keyfilter = filter("keyword").toString();
    QStringList keys = keyfilter.split(",");
    bool ok = true;
    for(const QString& key : keys){
        if ( keywords.indexOf(key) == -1){
            ok = false;
            break;
        }
    }
    return ok;
}
















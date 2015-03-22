#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include "kernel.h"
#include "connectorinterface.h"
#include "resource.h"
#include "ilwisobject.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "catalogmodel.h"


using namespace Ilwis;
//using namespace Desktopclient;

CatalogModel::CatalogModel()
{
    //_hasChilderen = false;
    _initNode = false;
    _level = 0;
    _isScanned = false;
}

CatalogModel::~CatalogModel()
{

}

CatalogModel::CatalogModel(const CatalogView &view, int lvl, QObject *parent) : ResourceModel(view.resource(), parent)
{
    _initNode = true;
    _level = lvl;
    _isScanned = true;
    newview(view);

}

void CatalogModel::newview(const CatalogView &view){
    _view = view;
    mastercatalog()->addContainer(view.resource().url());
    _displayName = view.resource().name();
    if ( _displayName == sUNDEF)
        _displayName = view.resource().url().toString();
}

bool CatalogModel::isScanned() const
{
    return _isScanned;
}

bool CatalogModel::initNode() const {
    return _initNode;
}

int CatalogModel::level() const
{
    return _level;
}

QQmlListProperty<ResourceModel> CatalogModel::resources() {

    try{
        gatherItems();

        return  QQmlListProperty<ResourceModel>(this, _currentItems);
    }
    catch(const ErrorObject& err){

    }
    return  QQmlListProperty<ResourceModel>();
}

QQmlListProperty<IlwisObjectModel> CatalogModel::selectedData()
{
    return  QQmlListProperty<IlwisObjectModel>(this, _selectedObjects);
}

void CatalogModel::makeParent(QObject *obj)
{
    setParent(obj);
}

void CatalogModel::filterChanged(const QString& objectType, bool state){
    if ( objectType == "all"){
        _filterState["rastercoverage"] = state;
        _filterState["featurecoverage"] = state;
        _filterState["table"] = state;
        _filterState["coordinatesystem"] = state;
        _filterState["georeference"] = state;
        _filterState["domain"] = state;
        _filterState["representation"] = state;
        _filterState["projection"] = state;
        _filterState["ellipsoid"] = state;

    }else
        _filterState[objectType] = state;
    QString filterString;
    for(auto iter : _filterState){
        if ( !iter.second){
            if ( filterString != "")
                filterString += " and ";
            filterString += QString("type") + "& '" + iter.first + "' =0";
        }
    }
    _refresh = true;
    _view.filter(filterString);
    contentChanged();
}

void CatalogModel::refresh(bool yesno)
{
    _refresh = yesno;
}

void CatalogModel::setSelectedObjects(const QString &objects)
{
    try {
    QStringList parts = objects.split("|");
    _selectedObjects.clear();
    for(auto objectid : parts){
        Resource resource = mastercatalog()->id2Resource(objectid.toULongLong());
        IlwisObjectModel *ioModel = new IlwisObjectModel(resource, this);
        if ( ioModel->isValid()){
            _selectedObjects.append(ioModel);
            emit selectionChanged();
        }else
            delete ioModel;
    }
    }catch(const ErrorObject& err){

    }
}

void CatalogModel::nameFilter(const QString &filter)
{
    _nameFilter = filter;
    _currentItems.clear();
    emit contentChanged();
}

QString CatalogModel::nameFilter() const
{
    return _nameFilter;
}

void CatalogModel::gatherItems() {
    if ( _currentItems.isEmpty() || _refresh) {
        if ( !_view.isValid())
            return;

        _view.prepare();

        _currentItems.clear();
        _refresh = false;

        std::vector<Resource> items = _view.items();
        for(const Resource& resource : items){
            if ( _nameFilter != ""){
                if ( resource.name().indexOf(_nameFilter) == -1){
                    if ( resource["longname"].toString().indexOf(_nameFilter) == -1)
                        continue;
                }
            }
            _currentItems.push_back(new ResourceModel(resource));
        }
    }
}





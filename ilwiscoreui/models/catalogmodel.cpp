#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include "coverage.h"
#include "connectorinterface.h"
#include "resource.h"
#include "georeference.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "catalogmodel.h"
#include "tranquilizer.h"
#include "layermanager.h"
#include "ilwiscontext.h"


using namespace Ilwis;
//using namespace Desktopclient;

CatalogModel::~CatalogModel()
{

}

CatalogModel::CatalogModel(QObject *parent) : ResourceModel(Resource(), parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
}

CatalogModel::CatalogModel(const Resource &res, QObject *parent) : ResourceModel(res,parent)
{
    _initNode = true;
    _isScanned = false;
    _level = 0;
    if ( res.url().toString() == Catalog::DEFAULT_WORKSPACE){
        _view = CatalogView(context()->workingCatalog()->source());
        setDisplayName("default");
    }else{
        _view = CatalogView(res);
        _displayName = _view.name();
    }
}

void CatalogModel::setView(const CatalogView &view){
    _view = view;
    resource(view.resource());
    mastercatalog()->addContainer(view.resource().url());
    _displayName = view.resource().name();
    if ( _displayName == sUNDEF)
        _displayName = view.resource().url().toString();
}

CatalogView CatalogModel::view() const
{
    return _view;
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

QQmlListProperty<CatalogMapItem> CatalogModel::mapItems()
{
   return  QQmlListProperty<CatalogMapItem>(this, _catalogMapItems);
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
    filter(filterString);
}

void CatalogModel::filter(const QString &filterString)
{
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
        if ( objects == ""){
            _selectedObjects.clear();
            return;
        }
        QStringList parts = objects.split("|");
        _selectedObjects.clear();
        kernel()->issues()->silent(true);
        for(auto objectid : parts){
            bool ok;
            Resource resource = mastercatalog()->id2Resource(objectid.toULongLong(&ok));
            if (!ok)
                continue;

            IlwisObjectModel *ioModel = new IlwisObjectModel(resource, this);
            if ( ioModel->isValid()){
                _selectedObjects.append(ioModel);
                emit selectionChanged();
            }else
                delete ioModel;
        }
        kernel()->issues()->silent(false);
    }catch(const ErrorObject& ){
    }catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    kernel()->issues()->silent(false);
}

QString CatalogModel::selectedIds() const
{
    QString selected;
    for(auto obj : _selectedObjects ){
        if ( selected != "")
            selected += "|";
        selected += obj->id();

    }
    return selected;
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

void CatalogModel::prepareMapItems(LayerManager *manager)
{
    try{
        if ( _catalogMapItems.size() == 0){
            kernel()->issues()->silent(true);
            for (auto iter  = _currentItems.begin(); iter != _currentItems.end(); ++iter){
                if(hasType((*iter)->type(), itCOVERAGE)){
                    Ilwis::ICoverage cov(((*iter)->resource()));
                    if ( cov.isValid() && cov->coordinateSystem().isValid())    {
                        if ( cov->coordinateSystem()->isLatLon() || cov->coordinateSystem()->canConvertToLatLon()){
                            _catalogMapItems.push_back(new CatalogMapItem(cov,manager->screenGrf(),this));
                        }
                    }
                }
            }
            kernel()->issues()->silent(false);
        }
    } catch (const Ilwis::ErrorObject& ){

    } catch (std::exception& ex){
        Ilwis::kernel()->issues()->log(ex.what());
    }
    kernel()->issues()->silent(false);
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
            _currentItems.push_back(new ResourceModel(resource, this));

        }
    }
}





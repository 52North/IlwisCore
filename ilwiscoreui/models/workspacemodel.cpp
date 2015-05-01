#include "kernel.h"
#include "workspacemodel.h"

WorkSpaceModel::WorkSpaceModel(const QString &name, QObject *parent) : CatalogModel(parent)
{
    setDisplayName(name);
}

WorkSpaceModel::~WorkSpaceModel()
{

}

void WorkSpaceModel::addItems(const QString& ids)
{
    QStringList idlist = ids.split("|");
    for(auto id : idlist){
        _view.addFixedItem(id.toULongLong());
    }
    refresh(true);
    emit dataChanged();
}

void WorkSpaceModel::removeItems(const QString& ids)
{
    QStringList idlist = ids.split("|");
    for(auto id : idlist){
        _view.removeFixedItem(id.toULongLong());
    }
}

void WorkSpaceModel::gatherItems() {
    bool needRefresh = _refresh;
    CatalogModel::gatherItems();
    if ( needRefresh){
        _operations.clear();
        _data.clear();
    }
    for(auto iter=_currentItems.begin(); iter != _currentItems.end(); ++iter){
        if ( (*iter)->type() == itOPERATIONMETADATA){
            _operations.push_back(*iter);
        }else if ( hasType((*iter)->type(), itILWISOBJECT)){
            _data.push_back(*iter);
        }
    }
    refresh(false);
}

QQmlListProperty<ResourceModel> WorkSpaceModel::operations()
{
    refresh(true);
    gatherItems();
    return  QQmlListProperty<ResourceModel>(this, _operations);
}

QQmlListProperty<ResourceModel> WorkSpaceModel::data()
{
    refresh(true);
    gatherItems();
    return  QQmlListProperty<ResourceModel>(this, _data);
}


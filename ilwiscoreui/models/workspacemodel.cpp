#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include "kernel.h"
#include "ilwiscontext.h"
#include "workspacemodel.h"

WorkSpaceModel::WorkSpaceModel(const QString &name, QObject *parent) :
    CatalogModel( Ilwis::Resource("ilwis://internalcatalog/workspaces/" + name,itWORKSPACE),parent)
{
}

WorkSpaceModel::~WorkSpaceModel()
{

}

void WorkSpaceModel::addItems(const QString& ids)
{
    auto putInConfig = [&](quint64 id, int count) {

        QString basekey = "users/" + Ilwis::context()->currentUser() + "/workspace-" + name();
        Ilwis::Resource res = Ilwis::mastercatalog()->id2Resource(id);
        if ( res.isValid()){
            QString key;
            if ( res.ilwisType() == itOPERATIONMETADATA){
                key = QString("%1/url-operation-%2").arg(basekey).arg(count);
                Ilwis::context()->configurationRef().addValue(QString("%1/operation-count").arg(basekey),QString::number(count));
            }
            else{
                key = QString("%1/url-data-%2").arg(basekey).arg(count);
                Ilwis::context()->configurationRef().addValue(QString("%1/data-count").arg(basekey),QString::number(count));
            }
            Ilwis::context()->configurationRef().addValue(key, res.url().toString());
        }
    };


    QSqlQuery stmt(Ilwis::kernel()->database());
    int count = 0;
    if (_view.fixedItemCount() == 0) {
        QString query = QString("Insert into workspaces (workspaceid) values(%1)").arg(_view.id());
        if(!stmt.exec(query))
            return;
    }
    // we dont want to add ids that are already in the list so we collect all ids that we have laready added
    std::set<QString> presentids;
    for(auto item : _operations){
        presentids.insert(item->id());
    }
    for(auto item : _data){
        presentids.insert(item->id());
    }
    QStringList idlist = ids.split("|");
    for(auto id : idlist){
        if ( presentids.find(id) != presentids.end())
            continue;

        quint64 idn = id.toULongLong();
        putInConfig(idn, count);
        _view.addFixedItem(idn);
        ++count;

        QString query = QString("Select workspaceid from workspace where workspaceid=%1 and itemid=%2").arg(_view.id()).arg(idn);
        if (stmt.exec(query)) { // already there
            if ( stmt.next()) {
                continue;
            }else{
                query = QString("Insert into workspace (workspaceid, itemid) values(%1,%2)").arg(_view.id()).arg(idn);
                if(!stmt.exec(query))
                    return;
            }
        }
    }
    QString availableWorkspaces = Ilwis::ilwisconfig("users/" + Ilwis::context()->currentUser() + "/workspaces",QString(""));
    if ( availableWorkspaces.indexOf(name()) == -1){
        if ( availableWorkspaces.size() > 0){
            availableWorkspaces += "|";
        }
        availableWorkspaces += name();
        Ilwis::context()->configurationRef().addValue("users/" + Ilwis::context()->currentUser() + "/workspaces",availableWorkspaces);
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

bool WorkSpaceModel::isDefault() const
{
    return resource().url().toString() == Ilwis::Catalog::DEFAULT_WORKSPACE;
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
            _operations.push_back(new OperationModel((*iter)->resource(),this));
        }else if ( hasType((*iter)->type(), itILWISOBJECT)){
            _data.push_back(*iter);
        }
    }
    refresh(false);
}

QQmlListProperty<OperationModel> WorkSpaceModel::operations()
{
    refresh(true);
    gatherItems();
    return  QQmlListProperty<OperationModel>(this, _operations);
}

QQmlListProperty<ResourceModel> WorkSpaceModel::data()
{
    refresh(true);
    gatherItems();
    return  QQmlListProperty<ResourceModel>(this, _data);
}


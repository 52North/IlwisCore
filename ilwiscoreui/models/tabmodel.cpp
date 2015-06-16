#include "tabmodel.h"
#include "datapanemodel.h"

TabModel::TabModel(QObject *parent) : QObject(parent)
{

}

TabModel::TabModel(const QString &url, const QString &componentUrl, QObject *parent) :
    QObject(parent),
    _componentUrl(componentUrl),
    _displayName(url)
{

}

QString TabModel::componentUrl() const
{
    return _componentUrl;
}

QString TabModel::displayName() const
{
    return _displayName;
}

void TabModel::setDisplayName(const QString &nm)
{
    _displayName = nm;
}

bool TabModel::canSeparate() const
{
    return _canSeparate;
}

void TabModel::setSelected(bool yesno)
{
    _selected = yesno;
    emit selectedChanged();
}

bool TabModel::selected() const
{
    return _selected;
}

void TabModel::selectTab(){
    SidePanelModel *sidemodel = qobject_cast<SidePanelModel *>(parent());
    if ( sidemodel){
        DataPaneModel *datapmodel = qobject_cast<DataPaneModel *>(sidemodel->parent());
        datapmodel->select(side()=="left",-1,false);
        setSelected(true);
    }
}

QString TabModel::side() const
{
    SidePanelModel *sidemodel = qobject_cast<SidePanelModel *>(parent());
    if ( sidemodel)
        return sidemodel->side();
    return "left";
}


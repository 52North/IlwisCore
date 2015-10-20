#include "catalogfiltermodel.h"

//using namespace Ilwis;
//using namespace Desktopclient;

CatalogFilterModel::CatalogFilterModel(QObject *parent) :
    QObject(parent),
    _selectionState(false),
    _query(sUNDEF),
    _name(sUNDEF),
    _readonly(true)
{
}

CatalogFilterModel::CatalogFilterModel(QObject *parent,const QString& query, const QString& name, const QString& icon, bool state) :
    QObject(parent),
    _selectionState(state),
    _query(query),
    _name(name),
    _icon(icon),
    _readonly(true)
{
}

CatalogFilterModel::CatalogFilterModel(QObject *parent, const QString &folder, const QString &name) :
    QObject(parent),
    _selectionState(true),
    _query("container='" + folder + "'"),
    _name(name),
    _url(folder),
    _readonly(true)
{

}

bool CatalogFilterModel::selectionState() const
{
    return _selectionState;
}

void CatalogFilterModel::setSelectionState(bool yesno)
{
    _selectionState = yesno;
    selectionStateChanged(_selectionState);
}

QString CatalogFilterModel::catalogQuery() const
{
    return _query;
}

void CatalogFilterModel::setCatalogQuery(const QString &stmt)
{
    _query = stmt;
    catalogQueryChanged(_query);
}

QString CatalogFilterModel::name() const
{
    return _name;
}

void CatalogFilterModel::setName(const QString &nm)
{
    _name = nm;
    nameChanged(_name);
}

QString CatalogFilterModel::icon() const
{
    return _icon;
}

void CatalogFilterModel::setIcon(const QString &icpath)
{
    _icon = icpath;
    iconChanged(_icon);
}

bool CatalogFilterModel::isReadOnly() const
{
    return _readonly;
}

void CatalogFilterModel::setReadOnly(bool yesno)
{
    _readonly = yesno;
    readOnlyChanged(_readonly);
}

QString CatalogFilterModel::url() const
{
    return _url;
}

#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "catalogquery.h"
#include "catalogexplorer.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "oshelper.h"

using namespace Ilwis;

const QString Catalog::DEFAULT_WORKSPACE = "ilwis://internalcatalog/workspaces/default";

Catalog::Catalog()
{
}

Catalog::Catalog(const Resource &resource) : IlwisObject(resource)
{
}

Catalog::~Catalog()
{

}

std::vector<Resource> Catalog::items() const
{
    if  ( _items.size() == 0){
         const_cast<Catalog *>(this)->_items = mastercatalog()->select(source().url(),"");
    }
    return _items;
}

quint32 Catalog::itemCount() const
{
    return _items.size();
}

void Catalog::scan()
{
    if (! connector().isNull()){
        if( connector()->loadData(this)){
            setValid(true);
        }
    }
}

bool Catalog::prepare(const IOOptions &options)
{
    QString scheme =  source().url().scheme();
    if ( !source().isValid() || scheme.size() <= 1)
        return ERROR2(ERR_ILLEGAL_VALUE_2,"url",source().url().toString());

//    if ( !mastercatalog()->knownCatalogContent(source().url())){
//            scan();
//    }

    return true;
}

bool Catalog::isValid() const
{
    return source().url().isValid() && IlwisObject::isValid();
}

IlwisTypes Catalog::ilwisType() const {
    return itCATALOG;
}

QString Catalog::resolve(const QString &nm, IlwisTypes tp) const
{
    if ( nm == sUNDEF)
        return sUNDEF;

    QString name = nm;
    name = Resource::quoted2string(name);
    name = OSHelper::neutralizeFileName(name);
    if ( name.contains(QRegExp("\\\\|/"))) { // is there already path info; check if it is the catalog
        QString query = QString("select resource from mastercatalog where resource = '%1' or rawresource='%1'").arg(name);
        InternalDatabaseConnection results(query);
        if ( results.next()) {
            return name;
        }
        // might have been a fragment
        QString resolvedName =  context()->workingCatalog()->source().url().toString() + "/" + name;
        query = QString("select resource from mastercatalog where resource = '%1' or rawresource='%1'").arg(resolvedName);
        results.exec(query);
        if ( results.next()) {
            return resolvedName;
        }

    }
    QString query = QString("select resource from mastercatalog where name = '%1' and (type & %2) != 0 and container='%3'").arg(name).arg(tp).arg(source().url().toString());
    if ( tp == itUNKNOWN) // incomplete info, we hope that the name will be unique. wrong selection must be handled at the caller side
        query = QString("select resource from mastercatalog where name = '%1' and container='%2'").arg(name, source().url().toString());
    InternalDatabaseConnection results;
    results.exec(query);
    if ( results.next()) {
        QSqlRecord rec = results.record();
        return rec.value(0).toString();
    } else {
        auto resolvedName = name;
        if ( context()->workingCatalog().isValid()) {
            resolvedName =  context()->workingCatalog()->source().url().toString() + "/" + name;
            query = QString("select propertyvalue from catalogitemproperties,mastercatalog \
                            where mastercatalog.resource='%1' and mastercatalog.itemid=catalogitemproperties.itemid\
                    and (mastercatalog.extendedtype & %2) != 0").arg(resolvedName).arg(tp);
            results.exec(query);
            if ( results.next()){ // if it is in the extended type than it is ok
                return resolvedName;
            }
        }
        return sUNDEF;
    }
}

QUrl Catalog::parentCatalog() const
{
    return _parent;
}

void Catalog::setParentCatalog(const QUrl &url)
{
    if ( isReadOnly())
        return;
    changed(true);

    _parent = url;
}


QUrl Catalog::filesystemLocation() const
{
    if (source().url().scheme() == "file")
        return source().url();
    else
        return context()->cacheLocation();
}

IlwisObject *Catalog::clone()
{
    Catalog *catalog = new Catalog();
    copyTo(catalog);

    return catalog;
}

IIlwisObject Catalog::createObject(const QVariant &indexValue) const
{
    bool ok;
    IIlwisObject obj(indexValue.toUInt(&ok));
    if (ok)
        return obj;
    return IIlwisObject();
}

Resource Catalog::item(quint32 index) const
{
    if ( index < _items.size())
        return _items[index];
    return Resource();
}

void Catalog::addItemsPrivate(const std::vector<Resource> &itemlist, bool doclear)
{
    if ( doclear)
        _items.resize(0);

    if(_items.size() == 0){
        _items = itemlist;
    }
    else {
        for(auto resource : itemlist){
            if(std::find(_items.begin(), _items.end(), resource) == _items.end())
                _items.push_back(resource);
        }
    }
}

void Catalog::copyTo(IlwisObject* obj){
    Locker<> lock(_mutex);
    IlwisObject::copyTo(obj);
    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->_parent = _parent;
    catalog->prepare();
}














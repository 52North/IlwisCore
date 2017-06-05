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
         const_cast<Catalog *>(this)->_items = mastercatalog()->select(resource().url(),"");
    }
    return _items;
}

quint32 Catalog::itemCount() const
{
    return _items.size();
}

void Catalog::scan()
{
    Locker<> lock(_mutex);

    if (! connector().isNull()){
        if( connector()->loadData(this)){
            setValid(true);
            changed(true);
        }
    }
}

bool Catalog::prepare(const IOOptions &options)
{
    QString scheme =  resource().url().scheme();
    if ( !resource().isValid() || scheme.size() <= 1)
        return ERROR2(ERR_ILLEGAL_VALUE_2,"url",resource().url().toString());

    return true;
}

bool Catalog::isValid() const
{
    return resource().url().isValid() && IlwisObject::isValid();
}

IlwisTypes Catalog::ilwisType() const {
    return itCATALOG;
}

QString Catalog::resolve(const QString &nm, IlwisTypes tp) const
{
    if ( nm == sUNDEF)
        return sUNDEF;
    quint64 id = IlwisObject::internalname2id(nm);
    if ( id != i64UNDEF){
        Resource res = mastercatalog()->id2Resource(id);
        if ( res.isValid())
            return res.url().toString();
    }

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
        QString resolvedName =  context()->workingCatalog()->resource().url().toString() + "/" + name;
        query = QString("select resource from mastercatalog where resource = '%1' or rawresource='%1'").arg(resolvedName);
        results.exec(query);
        if ( results.next()) {
            return resolvedName;
        }

    }
    QString query = QString("select resource from mastercatalog where name = '%1' and (type & %2) != 0 and container='%3'").arg(name).arg(tp).arg(resource().url().toString());
    if ( tp == itUNKNOWN) // incomplete info, we hope that the name will be unique. wrong selection must be handled at the caller side
        query = QString("select resource from mastercatalog where name = '%1' and container='%2'").arg(name, resource().url().toString());
    InternalDatabaseConnection results;
    results.exec(query);
    if ( results.next()) {
        QSqlRecord rec = results.record();
        return rec.value(0).toString();
    } else {
        QString query = QString("select resource from mastercatalog where code = '%1' and (type & %2) != 0").arg(name).arg(tp);
        results.exec(query);
        if ( results.next()) {
            QSqlRecord rec = results.record();
            return rec.value(0).toString();
        } else {
            auto resolvedName = name;
            if ( context()->workingCatalog().isValid()) {
                resolvedName =  context()->workingCatalog()->resource().url().toString() + "/" + name;
                query = QString("select propertyvalue from catalogitemproperties,mastercatalog \
                                where mastercatalog.resource='%1' and mastercatalog.itemid=catalogitemproperties.itemid\
                        and (mastercatalog.extendedtype & %2) != 0").arg(resolvedName).arg(tp);
                results.exec(query);
                if ( results.next()){ // if it is in the extended type than it is ok
                    return resolvedName;
                } else {
                    QString query = QString("select resource from mastercatalog where name = '%1' and (type & %2) != 0 and container='ilwis://internalcatalog'").arg(name).arg(tp);
                    results.exec(query);
                    if ( results.next()) {
                        QSqlRecord rec = results.record();
                        return rec.value(0).toString();
                    }
                }
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
    if (resource().url().scheme() == "file")
        return resource().url();
    else
        return context()->cacheLocation();
}

IlwisObject *Catalog::clone()
{
    Catalog *catalog = new Catalog();
    copyTo(catalog);

    return catalog;
}

void Catalog::unload()
{
    if ( !connector().isNull()){
        connector()->unload();
    }
}

std::vector<QString> Catalog::explorerProviders()
{
     CatalogConnector *cconnector = dynamic_cast< CatalogConnector *>(connector().data());
     if ( cconnector){
         return cconnector->providers();
     }
     return std::vector<QString>();

}

bool Catalog::isSpatialDataLocation(const QUrl &url)
{
    QString loc = url.toString();
    if ( loc.indexOf("ilwis://system/coverages") == 0)
        return true;
    if ( loc.indexOf("ilwis://system") == 0)
        return false;
    if ( loc.indexOf("ilwis://operations") == 0)
        return false;
    return true;
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



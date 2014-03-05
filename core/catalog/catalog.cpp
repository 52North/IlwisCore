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
#include "ilwiscontext.h"
#include "mastercatalog.h"

using namespace Ilwis;

Catalog::Catalog()
{
}

Catalog::Catalog(const Resource &resource) : IlwisObject(resource)
{
}

Catalog::~Catalog()
{

}

std::list<Resource> Catalog::items() const
{
    return mastercatalog()->select(source().url(), _filter);
}



bool Catalog::prepare(const QString& filter)
{
    QString scheme =  source().url().scheme();
    if ( !source().isValid() || scheme.size() <= 1)
        return ERROR2(ERR_ILLEGAL_VALUE_2,"url",source().url().toString());

    bool ok = mastercatalog()->addContainer(source().url());
    if (!ok)
        return false;

    CatalogQuery query;
    _filter = query.transformQuery(filter);

    QStringList parts = source().url().toString().split("/");
    QString cid = parts.back();
    if ( cid == "")
        cid = "Catalog";

    Identity::prepare();

    setName(cid);

    return true;
}

QString Catalog::type() const
{
    return "Catalog";
}


bool Catalog::isValid() const
{
    return source().url().isValid() && _filter != "";
}

IlwisTypes Catalog::ilwisType() const {
    return itCATALOG;
}

QString Catalog::resolve(const QString &name, IlwisTypes tp) const
{
    if ( name.contains(QRegExp("\\\\|/"))) { // is there already path info; check if it is the catalog
        QString query = QString("select resource from mastecatalog where resource = '%2'").arg(name);
        QSqlQuery results = kernel()->database().exec(query);
        if ( results.next()) {
            return name;
        }
    }
    QString query = QString("select resource from mastercatalog where name = '%1' and (type & %2) != 0 and container='%3'").arg(name).arg(tp).arg(source().url().toString());
    if ( tp == itUNKNOWN) // incomplete info, we hope that the name will be unique. wrong selection must be handled at the caller side
        query = QString("select resource from mastercatalog where name = '%1' and container='%2'").arg(name, source().url().toString());
    QSqlQuery results = kernel()->database().exec(query);
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
            auto viaExtType = kernel()->database().exec(query);
            if ( viaExtType.next()){ // if it is in the extended type than it is ok
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

void Catalog::copyTo(IlwisObject* obj){
    Locker lock(_mutex);
    IlwisObject::copyTo(obj);
    Catalog *catalog = static_cast<Catalog *>(obj);
    catalog->_filter = _filter;
    catalog->_parent = _parent;
}














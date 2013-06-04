#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "identity.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "connectorinterface.h"
#include "catalogconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "catalogquery.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"

using namespace Ilwis;

Catalog::Catalog(QObject *parent) :
    QObject(parent)
{
}

Catalog::Catalog(const Catalog &cat) : QObject(),
    Identity(cat.name(),cat.id(),cat.code(),cat.description()),
    _filter(cat._filter),
    _location(cat._location),
    _parent(cat._parent)
{
}

std::list<Resource> Catalog::items() const
{
    return mastercatalog()->select(_location, _filter);
}



bool Catalog::prepare(const QUrl &resource, const QString& filter)
{

    bool ok = mastercatalog()->addContainer(resource);
    if (!ok)
        return false;

    _location = resource;
    CatalogQuery query;
    _filter = query.transformQuery(filter);

    QStringList parts = resource.path().split("/");
    QString cid = parts.back();

    setName(cid);

    Identity::prepare();

    return true;
}

QString Catalog::type() const
{
    return "Catalog";
}


bool Catalog::isValid() const
{
    return _location.isValid() && _filter != "";
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
    QString query = QString("select resource from mastercatalog where name = '%1' and (type & %2) != 0 and container='%3'").arg(name).arg(tp).arg(_location.toString());
    if ( tp == itUNKNOWN) // incomplete info, we hope that the name will be unique. wrong selection must be handled at the caller side
        query = QString("select resource from mastercatalog where name = '%1' and container='%2'").arg(name, _location.toString());
    QSqlQuery results = kernel()->database().exec(query);
    if ( results.next()) {
        QSqlRecord rec = results.record();
        return rec.value(0).toString();
    } else {
        auto resolvedName = name;
        if ( context()->workingCatalog()) {
            resolvedName =  context()->workingCatalog()->location().toString() + "/" + name;
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

QUrl Catalog::location() const
{
    return _location;
}

QUrl Catalog::filesystemLocation() const
{
    if (_location.scheme() == "file")
        return _location;
    else
        return context()->temporaryWorkLocation();
}














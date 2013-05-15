#include <QRegExp>
#include <QFileInfo>
#include <QUrl>
#include <QStringList>
#include <QDir>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "identity.h"
#include "kernel.h"
#include "catalogitem.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"

using namespace Ilwis;

inline uint Ilwis::qHash(const Ilwis::CatalogItem& resource ){
    return (::qHash(resource.url().toString()) + ::qHash(resource.ilwisType())) / 2;
}

bool Ilwis::operator==(const Ilwis::CatalogItem& resource1, const Ilwis::CatalogItem& resource2 ){
    return (resource1.url() == resource2.url()) && ( resource1.ilwisType() & resource2.ilwisType());
}

bool Ilwis::operator<(const Ilwis::CatalogItem& resource1, const Ilwis::CatalogItem& resource2 ){
    bool ok1 = resource1.url() < resource2.url();
    if ( resource1.url() == resource2.url())
        return resource1.ilwisType() < resource2.ilwisType();
    return ok1;
}

CatalogItem::CatalogItem() : _size(0), _ilwtype(itUNKNOWN), _extendedType(itUNKNOWN)
{
}

CatalogItem::CatalogItem(const QUrl &url) : _size(0), _ilwtype(itUNKNOWN), _extendedType(itUNKNOWN)
{
    _resource = url;
    prepare();
}

CatalogItem::CatalogItem(const QSqlRecord &rec) : Identity(rec.value("name").toString(),
                                                           rec.value("itemid").toLongLong(),
                                                           rec.value("code").toString())
{
    _resource = rec.value("resource").toString();
    _container = rec.value("container").toString();
    _size = rec.value("size").toLongLong();
    _dimensions = rec.value("dimensions").toString();
    _ilwtype = rec.value("type").toLongLong();
    _extendedType = rec.value("extendedtype").toLongLong();
    //if ( (_ilwtype & itGEODETICDATUM) || (_ilwtype & itPROJECTION) || (_ilwtype & itELLIPSOID))
    //    setCode(name());

    QString query = QString("Select * from catalogitemproperties where itemid=%1").arg(id());
    QSqlQuery db(kernel()->database());
    if (!db.exec(query)) {
            kernel()->issues()->logSql(db.lastError());
    }
    while(db.next()){
        QSqlRecord rec = db.record();
        QString key = rec.value("propertyname").toString() ;
        QVariant value = rec.value("propertyvalue");
        _properties[key] = value;
    }
}

QString CatalogItem::operator [](const QString &prop) const
{
    QString property = prop.toLower();
    if (property == "id")
        return QString::number(id());
    if ( property == "name")
        return name();

    QHash<QString, QVariant>::const_iterator iter = _properties.constFind(property);
    if ( iter != _properties.end())
        return iter.value().toString();


    return sUNDEF;

}

QVariant &CatalogItem::operator [](const QString &property)
{
    return _properties[property];
}

bool CatalogItem::hasProperty(const QString &prop) const
{
    QString property = prop.toLower();
    auto iter = _properties.constFind(property);
    return iter != _properties.end();
}

QUrl CatalogItem::url() const
{
    return _resource;
}

QUrl CatalogItem::container() const
{
    return _container;
}

quint64 CatalogItem::size() const
{
    return _size;
}

QString CatalogItem::dimensions() const
{
    return _dimensions;
}

void CatalogItem::setContainer(const QUrl& url) {
    _container = url;
}

IlwisTypes CatalogItem::ilwisType() const
{
    return _ilwtype;
}

IlwisTypes CatalogItem::extendedType() const
{
    return _extendedType;
}

QString CatalogItem::type() const
{
    return "CatalogItem";
}

void CatalogItem::setIlwisType(IlwisTypes tp)
{
    if ( (tp & itILWISOBJECT) != 0 || tp == itCATALOG)
        _ilwtype = tp;
    else
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("type", "IlwisObject"),IssueObject::itWarning);
}

void CatalogItem::setExtendedType(IlwisTypes tp)
{
    _extendedType = tp;
}

void CatalogItem::prepare()
{
    Identity::prepare();
}

bool CatalogItem::store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const
{
    bool ok = true;
    queryItem.bindValue(":itemid", id());
    queryItem.bindValue(":name", name());
    queryItem.bindValue(":code", code());
    QString pp = container().toString();
    queryItem.bindValue(":container", container().toString());
    queryItem.bindValue(":resource", url().toString());
    queryItem.bindValue(":type", ilwisType());
    queryItem.bindValue(":extendedtype", _extendedType);
    queryItem.bindValue(":size", size());
    queryItem.bindValue(":dimensions", _dimensions);
    ok = queryItem.exec();
    if (!ok) {
        kernel()->issues()->logSql(queryProperties.lastError());
        return false;
    }

    for(QHash<QString, QVariant>::const_iterator  iter = _properties.constBegin(); iter != _properties.constEnd(); ++iter) {
        queryProperties.bindValue(":itemid", id());
        QString nameItem = iter.key();
        queryProperties.bindValue(":propertyname",nameItem);
        queryProperties.bindValue(":propertyvalue", iter.value().toString());
        ok = queryProperties.exec();
        if (!ok) {
            kernel()->issues()->logSql(queryProperties.lastError());
        }
    }
    return ok;

}

bool CatalogItem::isValid() const
{
    return ( name() != sUNDEF && _ilwtype != itUNKNOWN && _resource.isValid());
}





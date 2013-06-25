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
#include "resource.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"



using namespace Ilwis;

uint Ilwis::qHash2(const QUrl& url, IlwisTypes tp ){
    return (::qHash(url.toString()) + ::qHash(tp)) / 2;
}

uint Ilwis::qHash(const Ilwis::Resource& resource ){
    return qHash2(resource.url(), resource.ilwisType());
}

bool Ilwis::operator==(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 ){
    return (resource1.url() == resource2.url()) && ( resource1.ilwisType() & resource2.ilwisType());
}

inline bool Ilwis::operator<(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 ){
    bool ok1 = resource1.url() < resource2.url();
    if ( resource1.url() == resource2.url())
        return resource1.ilwisType() < resource2.ilwisType();
    return ok1;
}

Resource::Resource() : _size(0), _ilwtype(itUNKNOWN), _extendedType(itUNKNOWN)
{
}

Resource::Resource(const Resource &res) : Identity(res)
{
    _properties = res._properties;
    _resource = res._resource;
    _container = res._container;
    _size = res._size;
    _dimensions = res._dimensions;
    _ilwtype = res._ilwtype;
    _extendedType = res._extendedType;
}

Resource::Resource(const QString& code, quint64 tp, bool isNew) :
    _resource(QUrl(code)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    if ( tp == itUNKNOWN)
        return;

    int index = code.indexOf(":");
    if ( index != -1 && index < 6) {
        _resource = QUrl(code);
        checkUrl(tp);
    } else {
        //QString url;
        if ( code.left(4)=="code"){
            QString factoryType = sUNDEF;
            if ( tp & itCOORDSYSTEM)
                factoryType = "coordinatesystem";
            if ( tp & itPROJECTION)
                factoryType = "projection";
            if ( tp & itELLIPSOID)
                factoryType = "ellipsoid";
            QString c = QString("ilwis://factory/%1?%2").arg(factoryType).arg(code);
             _resource = QUrl(c);
        }
        checkUrl(tp);
    }

    if ( isNew)
        prepare();
}

Resource::Resource(const QUrl &url, quint64 tp, bool isNew) :
    _resource(url),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    if ( tp == itUNKNOWN)
        return;

    checkUrl(tp);
    if ( isNew)
        prepare();
}

Resource::Resource(quint64 tp, const QUrl &url) :
    _resource(url),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    if ( tp == itUNKNOWN)
        return;

    checkUrl(tp);
    prepare();
    if ( url == INTERNAL_OBJECT) {
        QString resext = url.toString() + "/" + name();
        _resource = QUrl(resext);
    }

}

Resource::Resource(const QSqlRecord &rec) : Identity(rec.value("name").toString(),
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

void Resource::setName(const QString &nm)
{
    Identity::setName(nm);
    if ( nm.indexOf(INTERNAL_PREFIX) != -1)  {
        QString url = _resource.toString();
        int index = url.lastIndexOf("/");
        if ( index != -1) {
            url = url.left(index+1) + nm;
        }
        _resource = QUrl(url);
    }
}

QVariant Resource::operator [](const QString &prop) const
{
    QString property = prop.toLower();
    if (property == "id")
        return QString::number(id());
    if ( property == "name")
        return name();

    QHash<QString, QVariant>::const_iterator iter = _properties.constFind(property);
    if ( iter != _properties.end())
        return iter.value();


    return sUNDEF;

}

bool Resource::hasProperty(const QString &prop) const
{
    QString property = prop.toLower();
    auto iter = _properties.constFind(property);
    return iter != _properties.end();
}

void Resource::addProperty(const QString &key, const QVariant &value)
{
    _properties[key] = value;
}

QUrl Resource::url() const
{
    return _resource;
}

void Resource::setUrl(const QUrl &url)
{
    _resource = url;
    QString urlTxt = url.toString();
    if ( urlTxt.indexOf("ilwis://operations/") == 0) {
        int index1 = urlTxt.indexOf("=");
        QString sname = urlTxt.mid(19,index1-19);
        setName(sname);
        int index2 = urlTxt.indexOf("/", index1);
        QString scode = urlTxt.mid(index1 + 1, index2 - index1);
        setCode(sname + "_" + scode );
        return;
    }
    QFileInfo inf(_resource.toLocalFile());
    if ( urlTxt != "file://") {
        if ( !url.hasFragment()) {
            setName(inf.fileName());
            _container = QUrl::fromLocalFile(inf.absolutePath());
        } else {
            QString fragment = url.fragment();
            QString fpath = fragment.split("=").back();
            bool ok;
            int index = fpath.toInt(&ok);
            if ( ok) { //TODO other cases than indexes; no example yet so postponed till there is one
                QString name = QString("%1_%2").arg(inf.fileName()).arg(index);
                setName(name);
                _container = QUrl(url.toString(QUrl::RemoveFragment));
            }

        }
    }
    else
        setName("root");
}

QUrl Resource::container() const
{
    return _container;
}

quint64 Resource::size() const
{
    return _size;
}

QString Resource::dimensions() const
{
    return _dimensions;
}

void Resource::setContainer(const QUrl& url) {
    _container = url;
}

IlwisTypes Resource::ilwisType() const
{
    return _ilwtype;
}

IlwisTypes Resource::extendedType() const
{
    return _extendedType;
}

void Resource::setIlwisType(IlwisTypes tp)
{
    if ( (tp & itILWISOBJECT) != 0 || tp == itCATALOG)
        _ilwtype = tp;
    else
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("type", "IlwisObject"),IssueObject::itWarning);
}

void Resource::setExtendedType(IlwisTypes tp)
{
    _extendedType = tp;
}

void Resource::prepare()
{
    Identity::prepare();
}

bool Resource::store(QSqlQuery &queryItem, QSqlQuery &queryProperties) const
{
    bool ok = true;

    queryItem.bindValue(":itemid", id());
    queryItem.bindValue(":name", name());
    queryItem.bindValue(":code", code());
    queryItem.bindValue(":container", container().toString());
    QString dum = url().toString();
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

bool Resource::isValid() const
{
    return ( name() != sUNDEF && _ilwtype != itUNKNOWN && _resource.isValid());
}

bool Resource::operator ()(const Resource &resource)
{
    return *this < resource;
}

QString Resource::toLocalFile(bool relative) const {
    return toLocalFile(_resource, relative);
}

QString Resource::toLocalFile(const QUrl& url, bool relative) {

    QFileInfo localFile(url.toLocalFile());
    if ( localFile.exists() && relative == false)
        return localFile.absoluteFilePath();

    if ( localFile.fileName() == "")
        return sUNDEF;
    QString currentDir = context()->workingCatalog()->filesystemLocation().toLocalFile();
    QString localDir = localFile.absolutePath();
    if ( currentDir == localDir) {
        if ( relative)
            return localFile.fileName();
    }
    return localFile.absoluteFilePath();
}

void Resource::checkUrl(IlwisTypes tp) {
    if ( mastercatalog()->contains(_resource, tp)) {
        Resource res = mastercatalog()->name2Resource(_resource.toString(), tp);
        *this = res;
        return;
    }
    else if ( _resource.scheme() == "file") {
        setUrl(_resource);
    }
    else if ( _resource.scheme() == "ilwis") {
        QString res = _resource.toString();
        int index = res.lastIndexOf("/");
        int index2 = res.lastIndexOf("code=");
        if ( index2 > index)
            index = index2 + 4;
        QString name = res.right(res.size() - index - 1);
        if ( index2 != -1)
            setCode(name);
        setName(name);
    }
}




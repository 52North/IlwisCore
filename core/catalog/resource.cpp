#include <QFileInfo>
#include <QUrlQuery>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "catalog.h"
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

Resource::Resource(const Resource &resource) : Identity(resource)
{
    _properties = resource._properties;
    _resource = resource._resource;
    _urlQuery = resource._urlQuery;
    _container = resource._container;
    _size = resource._size;
    _dimensions = resource._dimensions;
    _ilwtype = resource._ilwtype;
    _extendedType = resource._extendedType;
}

Resource::Resource(const QString& name, quint64 tp, bool isNew) :
    _resource(QUrl(name)),
    _urlQuery(QUrlQuery(name)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    if ( tp == itUNKNOWN)
        return;

    if ( isNew)
        prepare();

    int index = name.indexOf(":");
    if ( index != -1 && index < 6) {
        _resource = QUrl(name);
        stringAsUrl(name, tp, isNew);
    } else {
        //QString url;
        if ( name.left(4)=="code"){
            QString factoryType = sUNDEF;
            if ( tp & itCOORDSYSTEM)
                factoryType = "coordinatesystem";
            if ( tp & itPROJECTION)
                factoryType = "projection";
            if ( tp & itELLIPSOID)
                factoryType = "ellipsoid";
            QString c = QString("ilwis://factory/%1?%2").arg(factoryType).arg(name);
             _resource = QUrl(c);
        }else {
            if( isNew){
                if(!name.contains(QRegExp("\\\\|/")) && !name.contains("code=")){
                    QUrl urltxt(QString("ilwis://internalcatalog/%1").arg(name));
                    ICatalog workingCatalog = context()->workingCatalog();
                    if ( workingCatalog.isValid()){
                        QUrl url =  workingCatalog->filesystemLocation();
                        if ( url.isValid() && url.scheme() == "file"){
                            QString filepath = url.toLocalFile() + name;
                            if (QFileInfo(filepath).exists()){
                                urltxt = QUrl::fromLocalFile(filepath);
                            }
                        }
                    }
                    _resource = urltxt;
                }
            }
        }
        checkUrl(tp);
    }


}

Resource::Resource(const QUrl &url, quint64 tp, bool isNew) :
    _resource(url),
    _urlQuery(QUrlQuery(url)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    stringAsUrl(url.toString(), tp, isNew);
}

Resource::Resource(quint64 tp, const QUrl &url) :
    _resource(url),
    _urlQuery(QUrlQuery(url)),
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
    QString nm = _resource.toString();
    int index = nm.lastIndexOf("/");
    if ( index != -1){
        addContainer(nm.left(index));
    }

}

Resource::Resource(const QSqlRecord &rec) : Identity(rec.value("name").toString(),
                                                           rec.value("itemid").toLongLong(),
                                                           rec.value("code").toString())
{
    _resource = rec.value("resource").toString();
    _urlQuery = QUrlQuery(rec.value("urlquery").toString());
    addContainer(rec.value("container").toString());
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
        QString stringlist = value.toString();
        int n = 0;
        if ( ( n = stringlist.indexOf("STRINGLIST:")) == 0){
            stringlist = stringlist.mid(n);
            value = stringlist.split(",");
         }
         _properties[key] = value;
    }
}

void Resource::setName(const QString &nm, bool adaptUrl)
{
    Identity::setName(nm);
    if ( !adaptUrl || nm == sUNDEF)
        return;

    QString url = _resource.toString();
    int index = url.lastIndexOf("/");
    if ( index != -1 ) {
        url = url.left(index+1) + nm;
    }
    _resource = QUrl(url);
    _urlQuery = QUrlQuery(url);
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
    _container.clear();
    _resource = url;
    _urlQuery = QUrlQuery(url);
    QString urlTxt = url.toString();
    if ( urlTxt.indexOf("ilwis://operations/") == 0) {
        int index1 = urlTxt.indexOf("=");
        QString sname = urlTxt.mid(19,index1-19);
        setName(sname, false);
        int index2 = urlTxt.indexOf("/", index1);
        QString scode = urlTxt.mid(index1 + 1, index2 - index1);
        setCode(sname + "_" + scode );
        addContainer(QUrl("ilwis://operations"));
        return;
    }
    QFileInfo inf(_resource.toLocalFile());
    if ( urlTxt != "file://") {
        if ( !url.hasFragment()) {
            if ( url.scheme() == "file"){
                setName(inf.fileName(), false);
                addContainer(QUrl::fromLocalFile(inf.absolutePath()));
            } else {
                QString path = url.toString(QUrl::RemoveQuery | QUrl::RemoveFragment);
                int index = path.lastIndexOf("/");
                addContainer(path.left(index));
                setName(path.mid(index + 1),false);
            }
        } else {
            QString fragment = url.fragment();
            QString fpath = fragment.split("=").back();
            bool ok;
            int index = fpath.toInt(&ok);
            if ( ok) { //TODO: other cases than indexes; no example yet so postponed till there is one
                QString name = QString("%1_%2").arg(inf.fileName()).arg(index);
                setName(name, false);
                addContainer(QUrl(url.toString(QUrl::RemoveFragment)));
            }

        }

    }
    else
        setName("root", false);
}

QUrlQuery Resource::urlQuery() const
{
    return _urlQuery;
}

bool Resource::hasUrlQuery() const
{
    return !_urlQuery.isEmpty();
}

QUrl Resource::container(int level) const
{
    if ( level < _container.size())
        return _container[level];
    return QUrl();
}

quint64 Resource::size() const
{
    return _size;
}

QString Resource::dimensions() const
{
    return _dimensions;
}

void Resource::addContainer(const QUrl& url, int level) {
    if ( level < _container.size())
        _container[0] = url;
    else
        _container.push_back(url);
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
    queryItem.bindValue(":resource", url().toString());
    queryItem.bindValue(":urlquery", urlQuery().toString());
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
        QString v;
        if ( iter.value().type() == QMetaType::QStringList)
            v = "STRINGLIST:" + iter.value().toStringList().join(",");
        v = iter.value().toString();
        queryProperties.bindValue(":propertyvalue", v);
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

void Resource::setId(quint64 newid)
{
    Identity::setId(newid);
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

Resource Resource::copy(quint64 id) const
{
    Resource resource = *this;
    resource._id = id;
    resource.prepare(); // generate new id
    return resource;
}

void Resource::stringAsUrl(const QString &txt, IlwisTypes tp, bool isNew)
{
    if ( tp == itUNKNOWN)
        return;

    checkUrl(tp);
    if ( isNew)
        prepare();
    int index = txt.lastIndexOf("/");
    if ( index != -1){
        setName(txt.mid(index + 1));
        QString rest = txt.left(index);

        if ( _resource.scheme().size() < rest.size() - 2 )
            addContainer(rest);
    }
}

void Resource::checkUrl(IlwisTypes tp) {
    if ( mastercatalog()->contains(_resource, tp)) {
        Resource resource = mastercatalog()->name2Resource(_resource.toString(), tp);
        *this = resource;
        return;
    }
    else if ( _resource.scheme() == "file") {
        setUrl(_resource);
    }
    else if ( _resource.scheme() == "ilwis") {
        QString resource = _resource.toString();
        int index = resource.lastIndexOf("/");
        int index2 = resource.lastIndexOf("code=");
        if ( index2 > index)
            index = index2 + 4;
        QString name = resource.right(resource.size() - index - 1);
        if ( index2 != -1)
            setCode(name);
        setName(name, false);
    }
}




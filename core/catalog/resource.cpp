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
#include "proj4parameters.h"
#include "oshelper.h"



using namespace Ilwis;

uint Ilwis::qHash2(const QUrl& url, IlwisTypes tp ){
    return (::qHash(OSHelper::neutralizeFileName(url.toString())) + ::qHash(tp)) / 2;
}

uint Ilwis::qHash(const Ilwis::Resource& resource ){
    return qHash2(resource.url(), resource.ilwisType());
}

bool Ilwis::operator==(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 ){
    return (resource1.url() == resource2.url()) && ( resource1.ilwisType() & resource2.ilwisType());
}

bool Ilwis::operator!=(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 ){
    return !(resource1 == resource2);
}

bool Ilwis::operator<(const Ilwis::Resource& resource1, const Ilwis::Resource& resource2 ){
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
    _normalizedUrl = resource._normalizedUrl;
    _rawUrl = resource._rawUrl;
    _urlQuery = resource._urlQuery;
    _container = resource._container;
    _rawContainer = resource._rawContainer;
    _size = resource._size;
    _dimensions = resource._dimensions;
    _ilwtype = resource._ilwtype;
    _extendedType = resource._extendedType;
    _modifiedTime = resource._modifiedTime;
    _createTime = resource._createTime;
    _changed = resource._changed;
}

Resource::Resource(const QString& resourceName, quint64 tp, bool isNew) :
    _normalizedUrl(QUrl(OSHelper::neutralizeFileName(resourceName))),
    _urlQuery(QUrlQuery(resourceName)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    if ( tp == itUNKNOWN)
        return;

    if ( isNew)
        prepare();
    int index = resourceName.indexOf(":");
    if ( index != -1 && index < 6) {
        _normalizedUrl = QUrl(OSHelper::neutralizeFileName(resourceName));
        stringAsUrl(resourceName, tp, isNew);
    } else {
        //QString url;
        if ( resourceName.left(4)=="code"){
            QString factoryType = sUNDEF;
            if ( tp & itCOORDSYSTEM)
                factoryType = "coordinatesystem";
            if ( tp & itPROJECTION)
                factoryType = "projection";
            if ( tp & itELLIPSOID)
                factoryType = "ellipsoid";
            if ( tp & itGEOREF)
                factoryType = "georef";
            QString scode = resourceName.mid(5);
            int index = resourceName.indexOf(":");
            if ( index > 0) {
                scode = resourceName.mid(index+1);
            }
            code(scode);
            _normalizedUrl = QUrl(INTERNAL_CATALOG + "/" + this->name());
            if ( scode == "csy:unknown")
                name("unknown coordinate system", false);
            if ( scode == "grf:unspecified")
                name("unspecified georeference", false);
        }else {
            if( isNew){
                if(!resourceName.contains(QRegExp("\\\\|/")) && !resourceName.contains("code=")){
                    QUrl urltxt(QString(INTERNAL_CATALOG + "/%1").arg(resourceName));
                    ICatalog workingCatalog = context()->workingCatalog();
                    if ( workingCatalog.isValid()){
                        QUrl url =  workingCatalog->filesystemLocation();
                        if ( url.isValid() && url.scheme() == "file"){
                            QString filepath = OSHelper::neutralizeFileName(url.toLocalFile()) + "/" + resourceName;
                            if (QFileInfo(filepath).exists()){
                                urltxt = QUrl::fromLocalFile(filepath);
                                name(resourceName, false, false);
                            }else
                                name(resourceName);
                        }
                    }
                    _normalizedUrl = urltxt;
                    QString container = urltxt.adjusted(QUrl::RemoveFilename).toString(); // there is now a '/'at the end, remove it
                    _container = container.left(container.size() - 1);

                }
            }
        }
        checkUrl(tp);
    }
    _createTime = Time::now();
    if ( _container ==  INTERNAL_CATALOG_URL ||
         (_container.toString() == "ilwis://operations" && tp == itWORKFLOW)){
        QString path = OSHelper::neutralizeFileName(context()->persistentInternalCatalog().toString());
        _rawContainer = QUrl(path);
        _rawUrl = QUrl(path + "/" + name());
    }else if ( !_rawUrl.isValid()){
          QString c = _normalizedUrl.adjusted(QUrl::RemoveFilename).toString();
        _rawContainer = c.left(c.size() - 1);
        _rawUrl = _normalizedUrl;// for the moment, can always overrule it
    }
    changed(false);

}

Resource::Resource(const QUrl &url, quint64 tp, bool isNew) :
    _normalizedUrl(url),
    _rawUrl(url),
    _urlQuery(QUrlQuery(url)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    stringAsUrl(url.toString(), tp, isNew);
    _createTime = Time::now();
   changed(false);
}

Resource::Resource(const QUrl& normalizedUrl,const QUrl &rawurl, quint64 tp, bool isNew) :
    _normalizedUrl(normalizedUrl),
    _rawUrl(rawurl),
    _urlQuery(QUrlQuery(rawurl)),
    _size(0),
    _ilwtype(tp),
    _extendedType(itUNKNOWN)
{
    stringAsUrl(normalizedUrl.toString(), tp, isNew);
    _createTime = Time::now();
   changed(false);
}

Resource::Resource(quint64 tp, const QUrl &normalizedUrl, const QUrl& rawUrl) :
    _normalizedUrl(normalizedUrl),
    _rawUrl(rawUrl),
    _urlQuery(QUrlQuery(rawUrl)),
    _size(0),
    _ilwtype(tp & itILWISOBJECT),
    _extendedType(tp & ~( tp & itILWISOBJECT))
{
    if ( tp == itUNKNOWN)
        return;

    checkUrl(tp);
    prepare();
    if ( normalizedUrl == INTERNAL_CATALOG_URL) {
        QString resext = normalizedUrl.toString() + "/" + name();
        _normalizedUrl = QUrl(resext);
    }
    QString nm = _normalizedUrl.toString();
    int index = nm.lastIndexOf("/");
    if ( index != -1){
        addContainer(nm.left(index));
    }
    _createTime = Time::now();
   changed(false);
}

Resource::Resource(const QSqlRecord &rec) : Identity(rec.value("name").toString(),
                                                           rec.value("itemid").toLongLong(),
                                                           rec.value("code").toString(),
                                                           rec.value("description").toString())
{
    _normalizedUrl = rec.value("resource").toString();
    _rawUrl = rec.value("rawresource").toString();
    _urlQuery = QUrlQuery(rec.value("urlquery").toString());
    addContainer(rec.value("container").toString());
    addContainer(rec.value("rawcontainer").toString(), true);
    _size = rec.value("size").toLongLong();
    _dimensions = rec.value("dimensions").toString();
    _ilwtype = rec.value("type").toLongLong();
    _extendedType = rec.value("extendedtype").toLongLong();
    bool ok;
    _modifiedTime = rec.value("modifiedtime").toDouble(&ok);
    _createTime = rec.value("createtime").toDouble();

    QString query = QString("Select * from catalogitemproperties where itemid=%1").arg(id());
    InternalDatabaseConnection db;
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
    changed(false);
}

Resource::~Resource()
{

}
QString Resource::code() const
{
    return Identity::code();
}

void Resource::code(const QString &cde)
{
    if ( cde == code())
        return;
    if ( cde.indexOf("code=") == 0){
        int index = cde.indexOf(":");
        if ( index > 0) {
            Identity::code(cde.mid(index+1));
        }else {
            Identity::code(cde.mid(6));
        }
    }else
        Identity::code(cde);
    changed(true);
}

void Resource::setDescription(const QString &desc)
{
    if ( _description == desc)
        return;

    _description = desc;
    changed(true);
}

void Resource::name(const QString &nm, bool adaptNormalizedUrl,bool updateDatabase)
{
    if ( name() == nm)
        return;
    if ( nm == Identity::code() && name() != sUNDEF) //names and codes are usually different particular when there is al
        return;

    QString dummy = name();
    changed(true);
    QString n = nm;
    if ( nm.indexOf("://") > 0){
        int index = nm.lastIndexOf("/");
        n = nm.mid(index + 1);

    }
    Identity::name(n);
    if ( id() != iUNDEF && _modifiedTime != rUNDEF){ // if createtime is undefined we are creating a new resource so it will not be in the mastercatalog(yet), no update needed
        if ( updateDatabase)
            mastercatalog()->changeResource(id(), "name",n);
    }
    if ( !adaptNormalizedUrl || nm == sUNDEF)
        return;

    QString url = _normalizedUrl.toString();
    int index = url.lastIndexOf("/");
    if ( index != -1 ) {
        url = url.left(index+1) + n;
    }
    _normalizedUrl = QUrl(url);
    if (updateDatabase) {
        mastercatalog()->changeResource(id(), "resource",_normalizedUrl);
        mastercatalog()->changeResource(id(), "rawresource",_rawUrl);
    }
}

QString Resource::rawName() const
{
    QString url = _rawUrl.toString(QUrl::RemoveScheme|QUrl::RemovePort|QUrl::RemoveQuery);
    QStringList parts = url.split("/");
    if ( parts.size() > 0)
        return parts.last();
    return sUNDEF;
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
    if ( _properties[key.toLower()] == value)
        return;
    changed(true);
    _properties[key.toLower()] = value;
}

void Resource::removeProperty(const QString &key)
{
    if ( _properties.find(key) == _properties.end())
        return;

    changed(true);
    _properties.remove(key);
}

QUrl Resource::url(bool asRaw) const
{
    if ( asRaw && _rawUrl.isValid())
        return _rawUrl;
    return _normalizedUrl;
}

void Resource::setUrl(const QUrl &url, bool asRaw, bool updateDatabase)
{
    if ( url == _normalizedUrl && asRaw == false)
        return;
    if ( url == _rawUrl && asRaw)
        return;

    changed(true);
    if ( asRaw) {
        if ( url.scheme() == "ilwis"){
            // we dont want normalized paths to internal catalog in the raw url
            if ( url.toString().indexOf(INTERNAL_CATALOG_URL.toString())!= -1){
                QString name = url.fileName();
                _rawUrl = context()->persistentInternalCatalog().toString() + "/" + name;
            }else
               _rawUrl = url;
        }else
            _rawUrl = url;
        _urlQuery = QUrlQuery(_rawUrl);
    } else{
        if ( url.scheme() == "file"){
            // it might be a absolute path that points to the internalcatalog so we replace it we a correct normalized path
            QUrl internalCatalog = context()->persistentInternalCatalog();
            QFileInfo localInternal(internalCatalog.toLocalFile());
            QFileInfo localFile(url.toLocalFile());
            QString localPath = localFile.absolutePath();
            QString localInternalPath = localInternal.absoluteFilePath();
            if ( OSHelper::neutralizeFileName(localPath,true) == OSHelper::neutralizeFileName(localInternalPath,true)){
                _normalizedUrl = INTERNAL_CATALOG + "/" + localFile.fileName();
            }else
                _normalizedUrl = url;
        }else
            _normalizedUrl = url;
    }

    QString urlTxt = url.toString();
    if ( urlTxt.indexOf("ilwis://operations/") == 0) {
        int index1 = urlTxt.indexOf("=");
        QString sname = urlTxt.mid(19,index1-19);
        name(sname, false, updateDatabase);
        int index2 = urlTxt.indexOf("/", index1);
        QString scode = urlTxt.mid(index1 + 1, index2 - index1);
        code(sname + "_" + scode );
        addContainer(QUrl("ilwis://operations"));
        return;
    }
    if ( ilwisType() != itANY &&
         hasType(ilwisType(), itWORKFLOW) &&
         !asRaw && url.scheme() == "file")
    { // case of setting the path of a workflow; must ofc be normalized to operations catalog
        int index = url.toString().lastIndexOf("/");
        QString sname = url.toString().mid(index + 1);
        name(sname, false, updateDatabase);
        QString scode = sname + "_" + QString::number(id());
        code(scode);
        addContainer(QUrl("ilwis://operations"));
        _normalizedUrl = "ilwis://operations/" + sname;
    }else {
        QFileInfo inf(_normalizedUrl.toLocalFile());
        if ( urlTxt != "file://" && urlTxt != "file:///") {
            if ( !url.hasFragment()) {
                if ( url.scheme() == "file" && inf.isAbsolute()){
                    if ( !isRoot(inf.absolutePath())){
                        name(inf.fileName(), false, updateDatabase);
                        if ( !inf.isRoot())
                            addContainer(QUrl::fromLocalFile(OSHelper::neutralizeFileName(inf.absolutePath(), true)),asRaw);
                        else
                            addContainer(QUrl("file:///"),asRaw);
                    }
                } else {
                    QString path = url.toString(QUrl::RemoveQuery | QUrl::RemoveFragment);
                    int index = -1;
                    if ( asRaw && url.scheme() == "http"){
                        index = path.indexOf("?");
                    }else
                        index = path.lastIndexOf("/");
                    addContainer(path.left(index),asRaw);
                    QString oldName = name();
                    if ( !asRaw || ( asRaw && (oldName == "" || oldName == sUNDEF)))
                        name(path.mid(index + 1),false, updateDatabase);
                }
            } else {
                QString fragment = url.fragment();
                QString fpath = fragment.split("=").back();
                bool ok;
                int index = fpath.toInt(&ok);
                if ( ok) { //TODO: other cases than indexes; no example yet so postponed till there is one
                    QString rname = QString("%1_%2").arg(inf.fileName()).arg(index);
                    name(rname, false, updateDatabase);
                    addContainer(QUrl(url.toString(QUrl::RemoveFragment)), asRaw);
                }

            }

        }
        else
            name("root", false,updateDatabase);
    }
}

QUrlQuery Resource::urlQuery() const
{
    return _urlQuery;
}

bool Resource::hasUrlQuery() const
{
    return !_urlQuery.isEmpty();
}

QUrl Resource::container(bool asRaw) const
{
    if ( asRaw) {
        return _rawContainer;
    }
    return _container;
}

quint64 Resource::size() const
{
    return _size;
}

void Resource::size(quint64 objectsz)
{
    if ( _size == objectsz)
        return;

    changed(true);
    _size = objectsz;
}

QString Resource::dimensions() const
{
    return _dimensions;
}

void Resource::dimensions(const QString &dim)
{
    if ( dim == _dimensions)
        return;
   changed(true);
    _dimensions = dim;
}

void Resource::addContainer(const QUrl& url, bool asRaw) {

    if ( url == _container && asRaw == false)
        return;
    if ( url == _rawContainer && asRaw)
        return;

    if ( asRaw ){
        if ( url != INTERNAL_CATALOG_URL)
            _rawContainer = url;
        if ( !_container.isValid()){
            //the container must contain the normalized path so any real path to the internalcatalog is replaced by the normalized version
            if ( url == context()->persistentInternalCatalog())
                _container = INTERNAL_CATALOG_URL;
            else
                _container = url;
        }
    }
    else{
        _container = url;
        QUrl internalCatalog = context()->persistentInternalCatalog();
        QFileInfo localInternal(internalCatalog.toLocalFile());
        QString lf = url.toLocalFile();
        if ( lf != "") {
            QFileInfo localFile(url.toLocalFile());
            QString localPath = localFile.absoluteFilePath();
            QString localInternalPath = localInternal.absoluteFilePath();
            if ( OSHelper::neutralizeFileName(localPath,true) == OSHelper::neutralizeFileName(localInternalPath,true)){
                _container = INTERNAL_CATALOG_URL;
            }
        }
        if ( !_rawContainer.isValid()){
            // rawcontainer always have real paths to any references to the internal catalog must be replaced by their real path
            if ( url == INTERNAL_CATALOG_URL){
                _rawContainer = context()->persistentInternalCatalog();
            }else
                _rawContainer = url;
        }
    }
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
    if ( tp == _ilwtype)
        return;
    changed(true);
    if ( (tp & itILWISOBJECT) != 0 || tp == itCATALOG)
        _ilwtype = tp;
    else
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("type", "IlwisObject"),IssueObject::itWarning);
}

void Resource::setExtendedType(IlwisTypes tp)
{
    if ( _extendedType == tp)
        return;

    changed(true);
    _extendedType = tp;
}

void Resource::prepare(quint64 base)
{
    Identity::prepare(base);
}

bool Resource::store(InternalDatabaseConnection &queryItem, InternalDatabaseConnection &queryProperties) const
{
    bool ok = true;

    queryItem.bindValue(":itemid", id());
    queryItem.bindValue(":name", name());
    queryItem.bindValue(":code", Identity::code());
    queryItem.bindValue(":description", description());
    queryItem.bindValue(":container", OSHelper::neutralizeFileName(container().toString()));
    queryItem.bindValue(":rawcontainer", OSHelper::neutralizeFileName(container(true).toString()));
    queryItem.bindValue(":resource", OSHelper::neutralizeFileName(url().toString()));
    queryItem.bindValue(":rawresource", OSHelper::neutralizeFileName(url(true).toString()));
    queryItem.bindValue(":urlquery", urlQuery().toString());
    queryItem.bindValue(":type", ilwisType());
    queryItem.bindValue(":extendedtype", _extendedType);
    queryItem.bindValue(":size", size());
    queryItem.bindValue(":dimensions", _dimensions);
    queryItem.bindValue(":modifiedtime", (double)_modifiedTime);
    queryItem.bindValue(":createtime", (double)_createTime);
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
        if ( iter.value().type() == QVariant::StringList)
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

bool Resource::load(QDataStream &stream){

    Identity::load(stream);
    int sz;
    stream >> sz;
    for(int i = 0; i < sz; ++i){
        QVariant value;
        QString key;
        stream >> key;
        stream >> value;
        _properties[key] = value;
    }
    stream >> _normalizedUrl;
    stream >> _rawUrl;
    QString url;
    stream >> url;
    _container = url;
    stream >> url;
    _rawContainer = url;
    stream >> _size;
    stream >> _dimensions;
    stream >> _createTime;
    stream >> _modifiedTime;
    stream >> _ilwtype;
    stream >> _extendedType;
    changed(false);

    return true;
}

bool Resource::store(QDataStream &stream) const
{
    Identity::store(stream);
    stream << _properties.size();
    auto iter = _properties.begin();
    while ( iter != _properties.end()){
        stream << iter.key();
        stream << iter.value();
        ++iter;
    }
    stream << _normalizedUrl;
    stream << _rawUrl;
    stream << _container.toString();
    stream << _rawContainer.toString();
    stream << _size;
    stream << _dimensions;
    stream << _createTime;
    stream << _modifiedTime;
    stream << _ilwtype;
    stream << _extendedType;

    return true;


}

bool Resource::isValid() const
{
    return ( name() != sUNDEF && _ilwtype != itUNKNOWN && _normalizedUrl.isValid());
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
    return toLocalFile(_rawUrl, relative);
}

QString Resource::toLocalFile(const QUrl& url, bool relative, const QString& ext) {

    QFileInfo localFile(url.toLocalFile());
    QString localPath = sUNDEF;
    if ( localFile.exists() && relative == false){ // it is already a local file, so nothing more needed
        localPath =  localFile.absoluteFilePath();
    }else {
        if ( localFile.fileName() == "") // invalid
            return sUNDEF;
        // contains no path so we will add the path of the working folder to it to create a fully defined path
        QString currentDir = context()->workingCatalog()->filesystemLocation().toLocalFile();
        QString localDir = localFile.absolutePath();
        if ( currentDir == localDir) {
            if ( relative)
                localPath = localFile.fileName();
        }
        localPath = localFile.absoluteFilePath();
    }
    if ( ext != sUNDEF){ // if desired we can change the extension
        int index;

        if ( (index = localPath.lastIndexOf(".")) != -1)
            localPath = localPath.left(index);
        localPath += "." + ext;
    }
    QFileInfo inf(OSHelper::neutralizeFileName(localPath));
    if ( relative)
        return inf.fileName();
    return inf.absoluteFilePath();
}

Resource Resource::copy(quint64 id) const
{
    Resource resource = *this;
    resource._id = id;
    resource.prepare(); // generate new id
    return resource;
}

void Resource::setInternalCatalogPaths(IlwisTypes tp, const QString &txt)
{

    if ( hasType(tp,itCATALOG|itCATALOGVIEW)){
        _rawUrl = context()->persistentInternalCatalog();
        int index = _rawUrl.toString().lastIndexOf("/");
        _rawContainer = _rawUrl.toString().left(index);
        _container = "ilwis://";
        _normalizedUrl = INTERNAL_CATALOG_URL;
    }else {
        int index = txt.lastIndexOf("/");
        QString name = txt.mid(index + 1);
        if ( (index = name.lastIndexOf(".")) != -1){
            name = name.left(index);
        }
        name += ".ilwis";
        _rawUrl = context()->persistentInternalCatalog().toString() + "/" + name;
        _rawContainer = context()->persistentInternalCatalog();
        _container = INTERNAL_CATALOG_URL;
        _normalizedUrl = INTERNAL_CATALOG + "/" + name;
    }
}

void Resource::stringAsUrl(const QString &txt, IlwisTypes tp, bool isNew)
{
    if ( tp == itUNKNOWN)
        return;

    checkUrl(tp);
    if ( isNew)
        prepare(hasType(tp,itOPERATIONMETADATA) ? 0 : 1000000);
    int index = txt.lastIndexOf("/");
    if ( index != -1){ // name is by default the last part of the url
        name(txt.mid(index + 1));
        QString internal = context()->persistentInternalCatalog().toString();
        if ( txt == INTERNAL_CATALOG || txt.indexOf(OSHelper::neutralizeFileName(internal)) == 0){
            setInternalCatalogPaths(tp, txt);
            return;
        }
        if ( txt.indexOf("http://") == 0){
            if ( txt.indexOf("?") != -1){
                index = txt.indexOf("?");
            }
        }
        QString rest = txt.left(index); // the rest is the container
        if ( rest.right(1) == "/")
            rest = txt.left(index + 1);
        else if ( rest.right(1) == ":")
            rest = rest + "/";

        // we might be at the root; no need then to add containers as there are none
        if ( isRoot(txt))
            return;

        if (  isRoot(rest) && rest.endsWith("/") == false) // add the potential missing slash due to the 'left' of above (only windows?)
            rest += "/";

        if ( hasType(tp, itOPERATIONMETADATA) && tp !=itANY){
            if ( _rawUrl.scheme() == "file") {
                QString currentName = name();
                QString shortName = currentName.split(".")[0];
                name(shortName);
                setUrl( "ilwis://operations/" + shortName + "=" + QString::number(id()), false);
            }
        }else
            addContainer(rest);
    }
}

void Resource::checkUrl(IlwisTypes tp) {
    if ( mastercatalog()->contains(_normalizedUrl, tp)) {
        Resource resource = mastercatalog()->name2Resource(_normalizedUrl.toString(), tp);
        *this = resource;
        return;
    }
    else if ( _normalizedUrl.scheme() == "file") {
        setUrl(_normalizedUrl);
    }
    else if ( _normalizedUrl.scheme() == "ilwis") {
        if ( (name().indexOf(ANONYMOUS_PREFIX) == 0 ||  name().indexOf("code=") >= 0) && name().indexOf("proj4:") >= 0 )  {// name is set, so nothing meeded
            QString resource = _normalizedUrl.toString();
            int index = resource.lastIndexOf("/");
            int index2 = resource.lastIndexOf("code=");
            if ( index2 > index)
                index = index2 + 4;
            QString rname = resource.right(resource.size() - index - 1);
            QString newName = rname;
            if ((index = resource.indexOf("proj4:"))!=-1){
                QString proj4Part = resource.mid(index + 6);
                Proj4Def def = Proj4Parameters::lookupDefintion(proj4Part);
                if ( def._name != sUNDEF){
                    newName = def._name;
                    rname = def._epsg;
                }else
                    newName = "Unknown_csy_" + QString::number(id());
                code(rname);
            }
            else if ( index2 != -1){
                if ( (index2 = rname.indexOf(":")) != -1){ // we dont want vestiges of the code definition in the code
                    rname = rname.mid(index2 + 1);
                }
                code(rname);
            }

            name(newName, false);
        }
    }
}

bool Resource::isRoot(const QString& txt) {
    return txt.endsWith("//");
}

bool Resource::isRoot() const {
    return isRoot(_normalizedUrl.toString());
}

QString Resource::quoted2string(const QString& name){
    if ( name.size() == 0)
        return name;

    if (name[0] == '\"' && name[name.size()-1] == '\"'){
        if ( name.indexOf("url|") == 1)
        return name.mid(5,name.size() - 6);
    }
    return name;
}

Resource Resource::property2Resource(const QString& propertyName, IlwisTypes type) const{
    QVariant property = (*this)[propertyName];
    if ( !property.isValid() || property.isNull() )
        return Resource();
    bool ok;
    quint64 id = property.toULongLong(&ok);
    if ( ok){
        return mastercatalog()->id2Resource(id);
    }
    else
        return mastercatalog()->name2Resource(property.toString(), type);

    return Resource();
}


double Resource::modifiedTime() const
{

    return _modifiedTime;
}

void Resource::modifiedTime(const double &tme, bool force)
{
    if(context()->initializationFinished() || force){
        if (!force) // if we force the modified time we get it from probably the disk info; this doesnt make the object changed as its part of its data on disk
            _changed = true;
        if ( _modifiedTime != rUNDEF ){
            return;
            //qDebug() << "modifying" << url().toString() << ilwisType();
        }
        _modifiedTime = tme;
    }
}

double Resource::createTime() const
{
    return _createTime;
}

void Resource::createTime(const double &time)
{
    if ( _createTime == time)
        return;

    changed(true);
    _createTime = time;
}

bool Resource::hasChanged() const
{
    return _changed;
}

void Resource::changed(bool yesno)
{
    _changed = yesno;
    if ( yesno)
        modifiedTime(Ilwis::Time::now());
}

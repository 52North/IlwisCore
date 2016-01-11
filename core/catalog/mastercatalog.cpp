#include <QRegExp>
#include <QUrlQuery>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QSqlField>
#include "identity.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "oshelper.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "ilwiscontext.h"
#include "catalog.h"
#include "symboltable.h"
#include "operationExpression.h"



Ilwis::MasterCatalog *Ilwis::MasterCatalog::_masterCatalog = 0;
const QString Ilwis::MasterCatalog::MASTERCATALOG = "ilwis://mastercatalog";

using namespace Ilwis;

MasterCatalog* Ilwis::mastercatalog() {
    if (Ilwis::MasterCatalog::_masterCatalog == 0) {
        Ilwis::MasterCatalog::_masterCatalog = new Ilwis::MasterCatalog();
        Ilwis::MasterCatalog::_masterCatalog->prepare();

    }
    return Ilwis::MasterCatalog::_masterCatalog;
}

MasterCatalog::MasterCatalog()
{
}

MasterCatalog::~MasterCatalog()
{
    _lookup.clear();
    _knownHashes.clear();
    _catalogs.clear();
}

bool MasterCatalog::prepare()
{
    QSettings settings("52n","ilwis4");

    //TODO: this piece code must move to a more appropriate place; at this moment it
    // is not guaranteed that the catalogconnectors are already known due to the order of the plugins
    // as they are loaded.
//    QString local = settings.value("localfilesystem",QVariant(sUNDEF)).toString();

    return true;
}


bool MasterCatalog::addContainer(const QUrl &inlocation)
{
    //Locker<std::recursive_mutex> lock(_guard);
    if ( !inlocation.isValid()) // it is valid to try this with an empty url; just wont do anything
        return true;

    QString original = OSHelper::neutralizeFileName(inlocation.toString().trimmed());
    if ( Resource::isRoot(original))
        return true;

    QString loc = original;
    if ( loc[loc.size() - 1] == '/' ){
        loc = loc.left(loc.size() - 1);
        int count = loc.count("/");
        if ( loc.indexOf(":///") != -1){
            if ( count == 3)
                loc = original;
        }else if ( loc.indexOf("://") != -1){
            if ( count == 2){
                loc= original;
            }
        }
    }

    if ( loc.indexOf("ilwis://tables") == 0||
         loc.indexOf("ilwis://factory") == 0 ||
         loc == "file://" ||
         loc == "file:/" ||
         loc == "ilwis:/" ||
         loc == MASTERCATALOG ||
         loc.isEmpty())
        return true;
    QUrl location(loc);
    IOOptions options;
    if (location.hasQuery()) {
        QUrlQuery empty;
        QUrlQuery query(location);
        location.setQuery(empty);
        QList<QPair<QString,QString>> items = query.queryItems();
        std::for_each(items.begin(), items.end(), [&options](QPair<QString,QString> item) {
            options.addOption(QPair<QString, QVariant>(item.first, item.second));
        });
    }
    if ( _catalogs.find(location) != _catalogs.end())
        return true;

    Resource resource = name2Resource(location.toString());
    if ( !resource.isValid() || !hasType(resource.extendedType(), itCATALOG)){
         resource = Resource(location, itCATALOG);
    }

    ICatalog catalog(resource, options);
    if ( !catalog.isValid()){
        return false;
    }
    catalog->scan();

    addItems({catalog->source()});
    //if ( catalog->itemCount() > 0){
        _catalogs.insert(location);
   // }
    return true;
}

ESPIlwisObject MasterCatalog::get(const QUrl &resource, IlwisTypes type) const
{
    Locker<std::recursive_mutex> lock(_guard);
    quint64 id = url2id(resource, type);
    return get(id);
}

ESPIlwisObject MasterCatalog::get(quint64 id) const
{
    Locker<std::recursive_mutex> lock(_guard);
    if ( id != i64UNDEF) {
        auto iter = _lookup.find(id);
        if ( iter != _lookup.end())
            return iter.value();
    }
    return ESPIlwisObject();
}

bool MasterCatalog::contains(const QUrl& url, IlwisTypes type) const{
    Locker<std::recursive_mutex> lock(_guard);
    auto hash = Ilwis::qHash2(url, type);
    auto  iter = _knownHashes.find(hash);
    if ( iter != _knownHashes.end()) {
        auto id = url2id(url, type);
        if ( id != i64UNDEF)
            return true;
    }
    return false;
}

bool MasterCatalog::knownCatalogContent(const QUrl &path) const
{
    Locker<std::recursive_mutex> lock(_guard);

    return _catalogs.find(path) != _catalogs.end();
}

bool MasterCatalog::usesContainers(const QUrl &url) const
{
    return _containerExceptions.find(url.scheme()) == _containerExceptions.end();
}

void MasterCatalog::addContainerException(const QString &scheme)
{
    _containerExceptions.insert(scheme);
}



bool MasterCatalog::removeItems(const std::vector<Resource> &items){
    Locker<std::recursive_mutex> lock(_guard);

    for(const Resource &resource : items) {
        auto iter = _knownHashes.find(Ilwis::qHash(resource));
        if ( iter != _knownHashes.end()) {
            _knownHashes.erase(iter);
        }
        QString stmt = QString("DELETE FROM mastercatalog WHERE itemid = %1" ).arg(resource.id());
        InternalDatabaseConnection db;
        if(!db.exec(stmt)) {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
        stmt = QString("DELETE FROM catalogitemproperties WHERE itemid = %1").arg(resource.id());
        if(!db.exec(stmt)) {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
    }

    return true;
}

bool MasterCatalog::addItems(const std::vector<Resource>& items)
{
    Locker<std::recursive_mutex> lock(_guard);

    if( items.size() == 0) // nothing to do; not wrong perse
            return true;
    //InternalDatabaseConnection db("BEGIN IMMEDIATE TRANSACTION");
    InternalDatabaseConnection queryItem, queryProperties;

    bool ok = queryItem.prepare("INSERT INTO mastercatalog VALUES(\
                  :itemid,:name,:code,:description,:container,:rawcontainer,:resource,:rawresource,:urlquery,:type,:extendedtype, :size,:dimensions, :modifiedtime,:createtime \
                  )" );
    if (!ok) {
        kernel()->issues()->logSql(queryItem.lastError());
        return false;

    }

    ok = queryProperties.prepare("INSERT INTO catalogitemproperties VALUES(\
                   :propertyvalue,:propertyname,:itemid\
                 )" );
    if (!ok) {
        kernel()->issues()->logSql(queryItem.lastError());
        return false;
    }
    std::set<QUrl> containers;

    for(const Resource &resource : items) {
        if (!resource.isValid())
           continue;
        if (resource.url().toString().indexOf(ANONYMOUS_PREFIX)!= -1)
            continue;
        if ( mastercatalog()->contains(resource.url(), resource.ilwisType()))
          continue;

        _knownHashes.insert(Ilwis::qHash(resource));
        resource.store(queryItem, queryProperties);
        containers.insert(resource.container());
    }
   // db.exec("COMMIT TRANSACTION");

    for(auto container : containers)
        emit contentChanged(container);


    return true;

}

bool MasterCatalog::updateItems(const std::vector<Resource>& items)
{
    Locker<std::recursive_mutex> lock(_guard);
    InternalDatabaseConnection db("BEGIN IMMEDIATE TRANSACTION");
    if( items.size() == 0) // nothing to do; not wrong perse
            return true;

    InternalDatabaseConnection queryItem, queryProperties;

    bool ok = queryItem.prepare("UPDATE mastercatalog set name=:name, "
                                "code=:code, "
                                "container=:container, "
                                "description=:description, "
                                "rawcontainer=:rawcontainer,"
                                "resource=:resource, "
                                "rawresource=:rawresource, "
                                "urlquery=:urlquery, "
                                "type=:type, "
                                "extendedtype=:extendedtype, "
                                "size=:size, "
                                "dimensions=:dimensions, "
                                "modifiedtime=:modifiedtime, "
                                "createtime=:createtime "
                                "WHERE itemid=:itemid");
    if (!ok) {
        kernel()->issues()->logSql(queryItem.lastError());
        return false;

    }


    ok = queryProperties.prepare("INSERT INTO catalogitemproperties VALUES(\
                   :propertyvalue,:propertyname,:itemid\
                 )" );
    if (!ok) {
        kernel()->issues()->logSql(queryItem.lastError());
        return false;
    }

    for(const Resource &resource : items) {
        if (!resource.isValid())
           continue;

        InternalDatabaseConnection deleteQuery;
        deleteQuery.exec("DELETE from catalogitemproperties WHERE itemid=" + QString::number(resource.id())) ;
        resource.store(queryItem, queryProperties);
    }
    db.exec("COMMIT TRANSACTION");


    return true;

}

quint64 MasterCatalog::url2id(const QUrl &url, IlwisTypes tp, bool casesensitive) const
{
    Locker<std::recursive_mutex> lock(_guard);
    QString query = QString("select itemid,type from mastercatalog where (resource = '%1' or rawresource = '%1')").arg(url.toString());
    if (!casesensitive)
       query = QString("select itemid,type from mastercatalog where lower(resource) = '%1' or lower(rawresource) = '%1'").arg(url.toString().toLower()) ;
    InternalDatabaseConnection db(query);
    while ( db.next()) {
        auto rec = db.record();
        auto iid = rec.value(0).toLongLong();
        auto itype = rec.value(1).toLongLong();
        if ( (itype & tp) || tp == itUNKNOWN)
            return iid;
    }

    return i64UNDEF;

}

Resource MasterCatalog::id2Resource(quint64 iid) const {
    Locker<std::recursive_mutex> lock(_guard);
    auto query = QString("select * from mastercatalog where itemid = %1").arg(iid);
    InternalDatabaseConnection db(query);
    if ( db.next()) {
        auto rec = db.record();
        return Resource(rec);
    }
    return Resource();
}

quint64 MasterCatalog::name2id(const QString &name, IlwisTypes tp) const
{
    Locker<std::recursive_mutex> lock(_guard);
    quint64 id = IlwisObject::internalname2id(name);
    if ( id == i64UNDEF){
        Resource resource = name2Resource(name,tp);
        id =  resource.id();
    }
    return id;
}

bool MasterCatalog::changeResource(quint64 objectid, const QString &attribute, const QVariant &var, bool extended)
{
   Locker<std::recursive_mutex> lock(_guard);
    auto setExtended = [](quint64 objectid, const QString &attribute, const QVariant &var)->QString{
        QString statement;
        Resource res = mastercatalog()->id2Resource(var.toLongLong());
        if ( res.isValid() && res.hasProperty(attribute))
            statement = QString("update catalogitemproperties set propertyname='%1', propertyvalue='%2'' where itemid=%3").arg(attribute).arg(var.toString()).arg(objectid);
        else
            statement = QString("insert into catalogitemproperties (propertyvalue,propertyname,itemid) values('%1','%2',%3)").arg(var.toString()).arg(attribute).arg(objectid);
        return statement;

    };
    if ( objectid == iUNDEF || !var.isValid())
        return false;

    QString statement;
    if ( attribute == "name"){
        QString newname = var.toString();
        newname.replace("'","''");
        statement = QString("update mastercatalog set name= '%1' where itemid=%2").arg(newname).arg(objectid) ;
    }
    if ( extended){
        statement = setExtended(objectid, attribute,var);
    }
    InternalDatabaseConnection sqlPublic;
    bool ok = sqlPublic.exec(statement);
    if (!ok) {
        kernel()->issues()->logSql(sqlPublic.lastError());
        return false;
    }
    return true;
}

IlwisTypes MasterCatalog::id2type(quint64 iid) const {
   Locker<std::recursive_mutex> lock(_guard);
    QString query = QString("select type from mastercatalog where itemid = %1").arg(iid);
    InternalDatabaseConnection results(query);
    if ( results.next()) {
        return results.value(0).toLongLong();
    }
    return itUNKNOWN;
}


Resource MasterCatalog::name2Resource(const QString &name, IlwisTypes tp) const
{
   Locker<std::recursive_mutex> lock(_guard);

    if ( name == sUNDEF)
        return Resource();

    if ( tp == itUNKNOWN) { // no definitive type given we, try to find based on name/type combination. will work mostely but not guaranteed. this is anyway a last resort method
        std::vector<IlwisTypes> types { itRASTER, itFEATURE, itTABLE, itGEOREF, itCOORDSYSTEM, itDOMAIN};
        for(IlwisTypes type: types) {
            Resource resource = name2Resource(name, type);
            if (resource.isValid())
                return resource;
        }

        return Resource();
    }
    auto resolvedName = name2url(name, tp);
    if (!resolvedName.isValid())
        return Resource();

    resolvedName = OSHelper::neutralizeFileName(resolvedName.toString());
    auto query = QString("select * from mastercatalog where (resource = '%1' or rawresource = '%1') and (type & %2) != 0").arg(resolvedName.toString()).arg(tp);
    InternalDatabaseConnection db(query);
    if ( db.next()) {
        auto rec = db.record();
        return Resource(rec);

    } else {
        query = QString("select propertyvalue from catalogitemproperties,mastercatalog \
                        where ( mastercatalog.resource='%1' or mastercatalog.rawresource='%1') and mastercatalog.itemid=catalogitemproperties.itemid\
                and (mastercatalog.extendedtype & %2) != 0").arg(resolvedName.toString()).arg(tp);
        InternalDatabaseConnection db(query);
        bool isExternalRef = true;
        while ( db.next()){ // external reference finding
            isExternalRef = false;
            bool ok;
            auto propertyid = db.value(0).toLongLong(&ok);
            if (!ok) {
                kernel()->issues()->log(QString(TR("Invalid catalog property, mastercatalog corrupted : %1?").arg(name)),IssueObject::itWarning);
            }
            auto type = id2type(propertyid);
            if ( type & tp)
                return id2Resource(propertyid);
        }
        if ( !isExternalRef) { // it was not an external reference but an internal one; if it was external it will never come here
            // this is a new resource which only existed as reference but now gets real, so add it to the catalog
            Resource resource(QUrl(resolvedName), tp);
            const_cast<MasterCatalog *>(this)->addItems({resource});
            return resource;
        }
    }
    return Resource();
}

QUrl MasterCatalog::name2url(const QString &name, IlwisTypes tp) const{
   Locker<std::recursive_mutex> lock(_guard);

    if ( name.contains(QRegExp("\\\\|/"))) { // is there already path info; then assume it is already a unique resource
        bool ok = OSHelper::isAbsolute(name); // name might be a partial path
        if ( ok && !OSHelper::isFileName(name))
    // translate name to url; we have the following cases
    // * already is a path, no big deal; just make an url of it
    // * is a code; construct a path leading to the internal connector as all codes are handled there
    // * no, path, no code. So it must be in the current working catalog.
    // * backup case. we try it by name. may fail as names are not necessarily unique. system is robust enough to handle fail cases

            return name;
        if ( ok){
            return QUrl::fromLocalFile(name).toString();
        }else {
            QString resolvedName =  context()->workingCatalog()->source().url().toString() + "/" + name;
            return resolvedName;
        }

    }
    if ( name.indexOf("code=wkt:")==0) { // second case -- handle most codes cases
        auto code = name.right(name.size() - 5);
        auto wkt = code.mid(4);
        auto table = "projection";
        if ( tp & itELLIPSOID )
            table = "ellipsoid";
        else if ( tp & itGEODETICDATUM)
            table = "datum";
        //auto query = QString("select code from %1 where wkt='%2'").arg(table, wkt);
        auto query = QString("select code from %1 where wkt like '%%2%'").arg(table, wkt);
        InternalDatabaseConnection db(query);
        if ( db.next()) {
            QString res = QString("ilwis://tables/%1?code=%2").arg(table,db.value(0).toString());
            return res;
        }else {
            kernel()->issues()->log(TR(ERR_FIND_SYSTEM_OBJECT_1).arg(wkt));
            return QUrl();
        }
    } else if ( name.indexOf("code=epsg:")== 0 ) {
        auto code = name.right(name.size() - 5);
        return QString("ilwis://tables/projectedcsy?code=%1").arg(code);

    } else if ( name.left(6) == "code=proj4:") {
        auto code = name.right(name.size() - 5);
        return QString("ilwis://projection/code=%1").arg(code);
    } else if ( name.left(12) == "code=domain:") {
        QString shortname = name.mid(name.indexOf(":") + 1);
        if ( shortname == "text" || shortname == "color" || shortname == "colorpalette")
            return QString("ilwis://system/domains/code=domain:%1").arg(shortname);
        return QString("ilwis://tables/domain?code=%1").arg(shortname);
    }else if ( name.left(12) == "code=georef:") {
        QString shortname = name.mid(name.indexOf(":") + 1);
        return QString("ilwis://system/georefs/%1").arg(shortname);
    }else if ( name.left(9) == "code=csy:") {
        QString shortname = name.mid(name.indexOf(":") + 1);
        return QString("ilwis://system/coordinatesystems/%1").arg(shortname);
    }else if ( name.left(9) == "code=rpr:") {
        QString shortname = name.mid(name.indexOf(":") + 1);
        return QString("ilwis://tables/representation?code=%1").arg(shortname);
    }
    QString tt =  name.left(12);
    if ( context()->workingCatalog().isValid()) { // thirde case -- use the working catalog to extend the path
        auto resolvedName = context()->workingCatalog()->resolve(name, tp);
        if ( resolvedName != sUNDEF)
            return resolvedName;
    }
    auto code = name;
    if ( code.indexOf("code=") == 0)
        code = code.mid(5);

    // fourth case -- try name
    auto query = QString("select resource,type from mastercatalog where name = '%1' or code='%1'").arg(code);
    InternalDatabaseConnection db(query);
    db.exec();
    while ( db.next()) {
        auto rec = db.record();
        auto type = rec.value(1).toLongLong();
        if ( type & tp)
            return rec.value(0).toString();
    }
    return QUrl();

}

bool MasterCatalog::isRegistered(quint64 id) const
{
    Locker<std::recursive_mutex> lock(_guard);
    return _lookup.contains(id);
}

bool MasterCatalog::unregister(quint64 id)
{
    Locker<std::recursive_mutex> lock(_guard);
    QHash<quint64,ESPIlwisObject>::const_iterator iter = _lookup.find(id);
    if ( iter != _lookup.end()) {
        _lookup.remove(id);
    }

    return true;

}

std::vector<Resource> MasterCatalog::select(const QString &selection) const
{
    Locker<std::recursive_mutex> lock(_guard);
    QString query;
    if ( selection.indexOf("catalogitemproperties.") == -1)
        query = QString("select * from mastercatalog where  %2").arg(selection);
    else
        query = QString("select * from mastercatalog,catalogitemproperties where mastercatalog.itemid = catalogitemproperties.itemid and %2").arg(selection);

    InternalDatabaseConnection results(query);
    std::vector<Resource> items;
    while( results.next()) {
        QSqlRecord rec = results.record();
        items.push_back(Resource(rec));
    }
    return items;

}

std::vector<Resource> MasterCatalog::select(const QUrl &resource, const QString &selection) const
{
    Locker<std::recursive_mutex> lock(_guard);

    if ( resource == MASTERCATALOG)
        return select(selection);

    QString rest = selection == "" ? "" : QString("and (%1)").arg(selection);
    QString query;
    if ( selection.indexOf("operations") != -1)
        query = "select * from mastercatalog,catalogitemproperties where mastercatalog.container = 'ilwis://operations' and mastercatalog.itemid = catalogitemproperties.itemid and (mastercatalog.type=262144 or mastercatalog.type=36028797018963968) and catalogitemproperties.propertyname='keyword' and catalogitemproperties.propertyvalue like '% cross%'";
    if ( selection.indexOf("catalogitemproperties.") == -1)
        query = QString("select * from mastercatalog where container = '%1' %2").arg(resource.toString(), rest);
    else
        query = QString("select * from mastercatalog,catalogitemproperties where mastercatalog.container = '%1' and mastercatalog.itemid = catalogitemproperties.itemid %2").arg(resource.toString(), rest);

   // query = "select * from mastercatalog,catalogitemproperties where mastercatalog.container = 'ilwis://operations' and mastercatalog.itemid = catalogitemproperties.itemid";
    InternalDatabaseConnection results(query);
    std::vector<Resource> items;
    while( results.next()) {
        QSqlRecord rec = results.record();
        items.push_back(Resource(rec));
    }
    //kernel()->endClock();
    return items;

}

void MasterCatalog::registerObject(ESPIlwisObject &data)
{
    Locker<std::recursive_mutex> lock(_guard);
    if ( data.get() == 0) {
        QHash<quint64,ESPIlwisObject>::iterator iter = _lookup.find(data->id());
        data = iter.value();
    } else {
        if ( !data->isAnonymous())
            addItems({data->source()});
        _lookup[data->id()] = data;

    }
}



#ifdef QT_DEBUG

void MasterCatalog::dumpLookup() const
{
    for(auto pr : _lookup) {
        qDebug() << pr->name();
    }
}

quint32 MasterCatalog::usecount(quint64 id)
{
    auto iter = _lookup.find(id);
    if ( iter != _lookup.end()) {
        return iter.value().use_count();
    }
    return 0;
}

#endif

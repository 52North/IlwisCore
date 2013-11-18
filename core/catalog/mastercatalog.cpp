#include <QString>
#include <QUrl>
#include <QRegExp>
#include <QSharedPointer>
#include <QVector>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSettings>
#include <QSqlField>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "catalogconnector.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "ilwiscontext.h"


Ilwis::MasterCatalog *Ilwis::MasterCatalog::_masterCatalog = 0;

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

    //TODO this piece code must move to a more appropriate place; at this moment it
    // is not guaranteed that the catalogconnectors are already known due to the order of the plugins
    // as they are loaded.
//    QString local = settings.value("localfilesystem",QVariant(sUNDEF)).toString();

    return true;
}


bool MasterCatalog::addContainer(const QUrl &location)
{
    QString loc = location.toString();
    if ( loc.indexOf("ilwis://tables") == 0||
         loc.indexOf("ilwis://factory") == 0
         || loc.isEmpty())
        return true;
    if ( _catalogs.find(location) != _catalogs.end())
        return true;

    Resource resource(location, itANY); // the system determines if it is a real container; we dont know it here
    auto *cfactory = kernel()->factory<ConnectorFactory>("connectorfactory", resource);
    if (!cfactory) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Connector Factory",location.toString());
    }
    UPContainerConnector container(cfactory->createSuitable<ContainerConnector>(Resource(location, itCONTAINER)));
    if ( container == 0 || !container->isValid()) {
        return ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"Catalog connector", location.toString());
    }

    addItems({resource});
    _catalogs.insert(location);
    return container->prepare();
}


ESPIlwisObject MasterCatalog::get(const QUrl &resource, IlwisTypes type) const
{
    quint64 id = resource2id(resource, type);
    return get(id);
}

ESPIlwisObject MasterCatalog::get(quint64 id) const
{
    if ( id != i64UNDEF) {
        auto iter = _lookup.find(id);
        if ( iter != _lookup.end())
            return iter.value();
    }
    return ESPIlwisObject();
}

bool MasterCatalog::contains(const QUrl& url, IlwisTypes type) const{
    auto hash = Ilwis::qHash2(url, type);
    auto  iter = _knownHashes.find(hash);
    if ( iter != _knownHashes.end()) {
        auto id = resource2id(url, type);
        if ( id != i64UNDEF)
            return true;
    }
    return false;
}

bool MasterCatalog::removeItems(const QList<Resource> &items){
    for(const Resource &resource : items) {
        auto iter = _knownHashes.find(Ilwis::qHash(resource));
        if ( iter != _knownHashes.end()) {
            _knownHashes.erase(iter);
        }
        QString stmt = QString("DELETE FROM mastercatalog WHERE resource = %1" ).arg(resource.id());
        QSqlQuery db(kernel()->database());
        if(!db.exec(stmt)) {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
        stmt = QString("DELETE FROM catalogitemproperties WHERE resource = %1").arg(resource.id());
        if(!db.exec(stmt)) {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
    }

    return true;
}

bool MasterCatalog::addItems(const QList<Resource>& items)
{
    QSqlQuery queryItem(kernel()->database()), queryProperties(kernel()->database());

    bool ok = queryItem.prepare("INSERT INTO mastercatalog VALUES(\
                  :itemid,:name,:code,:container,:resource,:type,:extendedtype, :size,:dimensions \
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

    for(const Resource &resource : items) {
        //if ( _lookup.contains(resource.id()))
        //    continue;
        if ( mastercatalog()->contains(resource.url(), resource.ilwisType()))
            continue;

         _knownHashes.insert(Ilwis::qHash(resource));
        resource.store(queryItem, queryProperties);
    }


    return true;

}
quint64 MasterCatalog::resource2id(const QUrl &url, IlwisTypes tp) const
{
    auto query = QString("select itemid,type from mastercatalog where resource = '%1'").arg(url.toString());
    auto results = kernel()->database().exec(query);
    while ( results.next()) {
        auto rec = results.record();
        auto iid = rec.value(0).toLongLong();
        auto itype = rec.value(1).toLongLong();
        if ( (itype & tp) || tp == itUNKNOWN)
            return iid;
    }

    return i64UNDEF;

}

Resource MasterCatalog::id2Resource(quint64 iid) const {
    auto query = QString("select * from mastercatalog where itemid = %1").arg(iid);
    auto results = kernel()->database().exec(query);
    if ( results.next()) {
        auto rec = results.record();
        return Resource(rec);
    }
    return Resource();
}

quint64 MasterCatalog::name2id(const QString &name, IlwisTypes tp) const
{
    if ( name.indexOf(NAME_ALIAS) == 0) {
        QString sid = name.mid(SZ_NAME_ALIAS);
        bool ok;
        quint64 id = sid.toLongLong(&ok);
        if (ok){
            ESPIlwisObject data = mastercatalog()->get(id);
            if ( data.get() != 0) {
                return data->id();
            }
        }
    }
    if ( name.left(11) == ANONYMOUS_PREFIX) { // internal objects are not in the catalog
        QString sid = name.mid(11);
        bool ok;
        quint64 id = sid.toLongLong(&ok);
        if (ok){
            ESPIlwisObject data = mastercatalog()->get(id);
            if ( data.get() != 0) {
                return data->id();
            }
        }
    }
    Resource resource = name2Resource(name,tp);
    return resource.id();
}

IlwisTypes MasterCatalog::id2type(quint64 iid) const {
    QString query = QString("select type from mastercatalog where itemid = %1").arg(iid);
    QSqlQuery results = kernel()->database().exec(query);
    if ( results.next()) {
        return results.value(0).toLongLong();
    }
    return itUNKNOWN;
}


Resource MasterCatalog::name2Resource(const QString &name, IlwisTypes tp) const
{
    if ( tp == itUNKNOWN) {
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


    auto query = QString("select * from mastercatalog where resource = '%1' and (type & %2) != 0").arg(resolvedName.toString()).arg(tp);
    auto results = kernel()->database().exec(query);
    if ( results.next()) {
        auto rec = results.record();
        return Resource(rec);

    } else {
        query = QString("select propertyvalue from catalogitemproperties,mastercatalog \
                        where mastercatalog.resource='%1' and mastercatalog.itemid=catalogitemproperties.itemid\
                and (mastercatalog.extendedtype & %2) != 0").arg(resolvedName.toString()).arg(tp);
        auto viaExtType = kernel()->database().exec(query);
        bool isExternalRef = true;
        while ( viaExtType.next()){ // external reference finding
            isExternalRef = false;
            bool ok;
            auto propertyid = viaExtType.value(0).toLongLong(&ok);
            if (!ok) {
                kernel()->issues()->log(TR("Invalid catalog property, mastercatalog corrupted?"),IssueObject::itWarning);
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
    if ( name.contains(QRegExp("\\\\|/"))) { // is there already path info; then assume it is already a unique resource
        QUrl url(name);
        if ( url.scheme().size() > 1)
            return name;
        return QUrl::fromLocalFile(name).toString();
    }
    if ( name.indexOf("code=wkt:")==0) {
        auto code = name.right(name.size() - 5);
        auto wkt = code.mid(4);
        auto table = "projection";
        if ( tp & itELLIPSOID )
            table = "ellipsoid";
        else if ( tp & itGEODETICDATUM)
            table = "datum";
        auto query = QString("select code from %1 where wkt='%2'").arg(table, wkt);
        auto codes = kernel()->database().exec(query);
        if ( codes.next()) {
            return QString("ilwis://tables/%1?code=%2").arg(table,codes.value(0).toString());
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
        return QString("ilwis://internalcatalog/%1").arg(name);
    }
    if ( context()->workingCatalog()) {
        auto resolvedName = context()->workingCatalog()->resolve(name, tp);
        if ( resolvedName != sUNDEF)
            return resolvedName;
    }
    auto code = name;
    if ( code.indexOf("code=") == 0)
        code = code.mid(5);

    auto query = QString("select resource,type from mastercatalog where name = '%1' or code='%1'").arg(code);
    auto results = kernel()->database().exec(query);
    while ( results.next()) {
        auto rec = results.record();
        auto type = rec.value(1).toLongLong();
        if ( type & tp)
            return rec.value(0).toString();
    }
    return QUrl();

}

bool MasterCatalog::isRegistered(quint64 id) const
{
    return _lookup.contains(id);
}

bool MasterCatalog::unregister(quint64 id)
{
    QHash<quint64,ESPIlwisObject>::const_iterator iter = _lookup.find(id);
    if ( iter != _lookup.end()) {
        _lookup.remove(id);
    }

    return true;

}

std::list<Resource> MasterCatalog::select(const QUrl &resource, const QString &selection) const
{
    QString rest = selection == "" ? "" : QString("and (%1)").arg(selection);
    QString query = QString("select * from mastercatalog where container = '%1' %2").arg(resource.toString(), rest);
    QSqlQuery results = kernel()->database().exec(query);
    std::list<Resource> items;
    while( results.next()) {
        QSqlRecord rec = results.record();
        items.push_back(Resource(rec));
    }
    return items;

}

void MasterCatalog::registerObject(ESPIlwisObject &data)
{
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

#endif

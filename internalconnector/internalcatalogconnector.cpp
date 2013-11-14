#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QSqlRecord>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "containerconnector.h"
#include "catalogconnector.h"
#include "internalcatalogconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalCatalogConnector::create(const Resource&, bool ) {
    return new InternalCatalogConnector();

}

InternalCatalogConnector::InternalCatalogConnector() :ContainerConnector(Resource(QUrl("ilwis://internalcatalog"),itCONTAINER))
{
}

bool InternalCatalogConnector::prepare()
{
    QSqlQuery db(kernel()->database());
    bool ok = createItems(db,"projection", itPROJECTION);
    ok &= createItems(db,"ellipsoid", itELLIPSOID);
    ok &= createItems(db,"datum", itGEODETICDATUM);
    ok &= createItems(db,"numericdomain", itNUMERICDOMAIN);
    ok &= createPcs(db);
    ok &= createSpecialDomains();

    return ok;

}

std::vector<QUrl> InternalCatalogConnector::sources(const QStringList &, int ) const
{
    //TODO full list??
    return std::vector<QUrl>();
}

bool InternalCatalogConnector::createSpecialDomains() {
    QString url = QString("ilwis://internalcatalog/code=domain:text");
    Resource resource(url, itTEXTDOMAIN);
    resource.setCode("text");
    resource.setName("Text domain", false);
    resource.addContainer(QUrl("ilwis://internalcatalog"));
    resource.prepare();
    return mastercatalog()->addItems({resource});
}

bool InternalCatalogConnector::canUse(const Resource &res) const
{
    return res.url().scheme() == "ilwis";
}

QString InternalCatalogConnector::provider() const
{
    return "internal";
}

QFileInfo InternalCatalogConnector::toLocalFile(const QUrl& url) const {
    QFileInfo inf = url.toLocalFile();
    return inf;
}

bool InternalCatalogConnector::isValid() const{
    return true;
}
bool InternalCatalogConnector::createPcs(QSqlQuery& db) {
    QString query = QString("Select * from projectedcsy");
    if ( db.exec(query)) {
        QList<Resource> items;
        while (db.next()) {
            QSqlRecord rec = db.record();
            QString code = rec.value("code").toString();
            QString name = rec.value("name").toString();
            QString url = QString("ilwis://tables/projectedcsy?code=%1").arg(code);
            Resource resource(url, itCONVENTIONALCOORDSYSTEM);
            resource.setCode(code);
            resource.setName(name, false);
            resource["wkt"] = name;
            resource.addContainer(QUrl("ilwis://system"));
            items.push_back(resource);
        }
        return mastercatalog()->addItems(items);
    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;
}

bool InternalCatalogConnector::createItems(QSqlQuery& db, const QString& table, IlwisTypes type) {
    QString query = QString("Select * from %1").arg(table);
    if ( db.exec(query)) {
        QList<Resource> items;
        while (db.next()) {
            QSqlRecord rec = db.record();
            QString code = rec.value("code").toString();
            IlwisTypes extType = rec.value("extendedtype").toLongLong();
            QString url = QString("ilwis://tables/%1?code=%2").arg(table,code);
            Resource resource(url, type);
            if ( type == itNUMERICDOMAIN) // for valuedomain name=code
                resource.setName(rec.value("code").toString(), false);
            else
                resource.setName(rec.value("name").toString(), false);

            resource.setCode(code);
            resource.setExtendedType(extType);
            resource.setDescription(rec.value("description").toString());
            resource.addContainer(QUrl("ilwis://system"));
            QString wkt = rec.value("wkt").toString();
            if ( wkt != "" && wkt != sUNDEF)
                resource["wkt"] = wkt;
            items.push_back(resource);
        }
        return mastercatalog()->addItems(items);
    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;
}

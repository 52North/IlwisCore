#include <QSqlQuery>
#include <QSqlError>
#include <QUrl>
#include <QSqlRecord>
#include "kernel.h"
#include "identity.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "catalogconnector.h"
#include "internalcatalogconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *InternalCatalogConnector::create(const Resource&, bool ) {
    return new InternalCatalogConnector();

}

InternalCatalogConnector::InternalCatalogConnector() : CatalogConnector(Resource(QUrl(),itUNKNOWN))
{
}

bool InternalCatalogConnector::loadItems()
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

bool InternalCatalogConnector::createSpecialDomains() {
    QString url = QString("ilwis://internal/code=domain:text");
    Resource item(url, itTEXTDOMAIN);
    item.setCode("text");
    item.setName("Text domain", false);
    item.setContainer(QUrl("ilwis://system"));
    item.prepare();
    return mastercatalog()->addItems({item});
}

bool InternalCatalogConnector::canUse(const QUrl &res) const
{
    return res.scheme() == "ilwis";
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
            Resource item(url, itCONVENTIONALCOORDSYSTEM);
            item.setCode(code);
            item.setName(name, false);
            item["wkt"] = name;
            item.setContainer(QUrl("ilwis://system"));
            items.push_back(item);
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
            Resource item(url, type);
            if ( type == itNUMERICDOMAIN) // for valuedomain name=code
                item.setName(rec.value("code").toString(), false);
            else
                item.setName(rec.value("name").toString(), false);

            item.setCode(code);
            item.setExtendedType(extType);
            item.setDescription(rec.value("description").toString());
            item.setContainer(QUrl("ilwis://system"));
            QString wkt = rec.value("wkt").toString();
            if ( wkt != "" && wkt != sUNDEF)
                item["wkt"] = wkt;
            items.push_back(item);
        }
        return mastercatalog()->addItems(items);
    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;
}

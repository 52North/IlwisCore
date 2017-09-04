#include <QSqlDatabase>

#include "databaseconnectionfactory.h"


QSqlDatabase DatabaseConnectionFactory::createDatabaseConnection(Resource &resource)
{
    _db = QSqlDatabase::addDatabase("QPSQL");
    QString url = resource.url().toString();
    QStringList parts = url.split("/");
    db.setHostName(parts[2]);
    db.setDatabaseName(parts[3]);

    QUrlQuery query = resource.urlQuery();
    db.setUserName(query.queryItemValue("user"));
    db.setPassword(query.queryItemValue("password"));
}

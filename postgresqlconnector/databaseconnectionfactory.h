#ifndef DATABASECONNECTIONFACTORY_H
#define DATABASECONNECTIONFACTORY_H

class DatabaseConnectionFactory
{
public:
    static QSqlDatabase createDatabaseConnection(Resource& resource);
};

#endif // DATABASECONNECTIONFACTORY_H

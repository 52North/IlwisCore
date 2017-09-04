#ifndef POSTGRESQLPARAMETERS_H
#define POSTGRESQLPARAMETERS_H

#include "kernel.h"

namespace Ilwis {
namespace Postgresql {

class PostgresqlParameters
{
public:
    PostgresqlParameters();
    PostgresqlParameters(const IOOptions &options);
    PostgresqlParameters(const QString & user, const QString & pass, const QString & host, const quint32 & port, const QString database = "", const QString schema = "", const QString table = "", const QString column = "", const QString rasterid = "");
    PostgresqlParameters(const QString & str);
    ~PostgresqlParameters();
    QString toString() const;
    QString toStringBasics() const; // only user,pass,host,port,database (which uniquely identifies a db-connection)
    bool valid() const;
    void toIOOptions(IOOptions & options) const;
    QString user() const;
    QString pass() const;
    QString host() const;
    quint32 port() const;
    QString database() const;
    QString schema() const;
    QString table() const;
    QString column() const;
    QString rasterID() const;
    /*
    void setUser(const QString &user);
    void setPass(const QString &pass);
    void setHost(const QString &host);
    void setPort(const quint32 &port);
    */
    void setDatabase(const QString &database);
    void setSchema(const QString &schema);
    void setTable(const QString &table);
    void setColumn(const QString &column);
    void setRasterID(const QString &rasterid);

private:
    void warnEmptyBasics() const;
    QString merge(QString left, QString right, QString delimiter) const;
    void split(QString fileName, QString & left, QString & right, QString delimiter) const;
    void replaceString(QString &str, const QString &search, const QString &replace) const;
    QString _user;
    QString _pass;
    QString _host;
    quint32 _port;
    QString _database;
    QString _schema;
    QString _table;
    QString _column;
    QString _rasterid;
};
}
}

#endif // POSTGRESQLPARAMETERS_H

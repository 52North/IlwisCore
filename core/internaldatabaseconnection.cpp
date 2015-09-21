#include "kernel.h"
#include "publicdatabase.h"
#include "internaldatabaseconnection.h"

using namespace Ilwis;

quint64 InternalDatabaseConnection::_locknumber;

InternalDatabaseConnection::InternalDatabaseConnection()
{
    Locker<std::recursive_mutex> lock(_guard);
    _connection = QSqlQuery(kernel()->database());
    _locknumber++;
}

InternalDatabaseConnection::InternalDatabaseConnection(const QString &query)
{
    Locker<std::recursive_mutex> lock(_guard);
    _connection = QSqlQuery(kernel()->database());
    _locknumber++;
    exec(query);
}

InternalDatabaseConnection::~InternalDatabaseConnection()
{

}

bool InternalDatabaseConnection::exec(const QString &query)
{
    return _connection.exec(query);
}

bool InternalDatabaseConnection::next()
{
    return _connection.next();
}

QSqlError InternalDatabaseConnection::lastError() const
{
    return _connection.lastError();
}

QVariant InternalDatabaseConnection::value(int i) const
{
    return _connection.value(i);
}

QVariant InternalDatabaseConnection::value(const QString &name) const
{
    return _connection.value(name);
}

QSqlRecord InternalDatabaseConnection::record() const
{
    return _connection.record();
}

bool InternalDatabaseConnection::exec()
{

    return _connection.exec();
}

bool InternalDatabaseConnection::prepare(const QString &query)
{
    return _connection.prepare(query);
}

void InternalDatabaseConnection::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType type)
{
    return _connection.bindValue(placeholder, val, type);
}

bool InternalDatabaseConnection::isValid() const
{
    return _connection.isValid();
}

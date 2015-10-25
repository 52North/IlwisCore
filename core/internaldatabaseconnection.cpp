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
   Locker<std::recursive_mutex> lock(_guard);
}

bool InternalDatabaseConnection::exec(const QString &query)
{
    Locker<std::recursive_mutex> lock(_guard);
    return _connection.exec(query);
}

bool InternalDatabaseConnection::next()
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.next();
}

QSqlError InternalDatabaseConnection::lastError() const
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.lastError();
}

QVariant InternalDatabaseConnection::value(int i) const
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.value(i);
}

QVariant InternalDatabaseConnection::value(const QString &name) const
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.value(name);
}

QSqlRecord InternalDatabaseConnection::record() const
{
    Locker<std::recursive_mutex> lock(_guard);
    return _connection.record();
}

bool InternalDatabaseConnection::exec()
{
   Locker<std::recursive_mutex> lock(_guard);
    return _connection.exec();
}

bool InternalDatabaseConnection::prepare(const QString &query)
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.prepare(query);
}

void InternalDatabaseConnection::bindValue(const QString &placeholder, const QVariant &val, QSql::ParamType type)
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.bindValue(placeholder, val, type);
}

bool InternalDatabaseConnection::isValid() const
{
       Locker<std::recursive_mutex> lock(_guard);
    return _connection.isValid();
}

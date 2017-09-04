#include "postgresqlparameters.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlParameters::PostgresqlParameters()
{
}

PostgresqlParameters::PostgresqlParameters(const IOOptions &options)
{
    _user = options.contains("pg.username") ? options["pg.username"].toString() : "";
    _pass = options.contains("pg.password") ? options["pg.password"].toString() : "";
    _host = options.contains("host") ? options["host"].toString() : "";
    _port = options.contains("port") ? options["port"].toInt() : 0;
    _database = options.contains("database") ? options["database"].toString() : "";
    _schema = options.contains("pg.schema") ? options["pg.schema"].toString() : "";
    _table = options.contains("pg.table") ? options["pg.table"].toString() : "";
    _column = options.contains("pg.column") ? options["pg.column"].toString() : "";
    _rasterid = options.contains("pg.rasterid") ? options["pg.rasterid"].toString() : "";
    warnEmptyBasics();
}

PostgresqlParameters::PostgresqlParameters(const QString & user, const QString & pass, const QString & host, const quint32 & port, const QString database, const QString schema, const QString table, const QString column, const QString rasterid)
: _user(user)
, _pass(pass)
, _host(host)
, _port(port)
, _database(database)
, _schema(schema)
, _table(table)
, _column(column)
, _rasterid(rasterid)
{
    warnEmptyBasics();
}

PostgresqlParameters::PostgresqlParameters(const QString & str)
{
    int index = str.indexOf("postgresql://");
    if ( index == 0) {
        QString rest = str.mid(13);
        index = rest.lastIndexOf("@"); // password may contain @
        if (index > 0) {
            QString userpass = rest.left(index);
            QStringList parts = rest.mid(index + 1).split("/");
            index = userpass.indexOf(":"); // user can't contain :
            _user = userpass.left(index);
            _pass = userpass.mid(index + 1);
            if (parts.size() > 0) {
                QStringList hostport = parts[0].split(":");
                if (hostport.size() > 0) {
                    _host = hostport[0];
                    if (hostport.size() > 1)
                        _port = hostport[1].toInt();
                }
                if (parts.size() > 1) {
                    _database = parts[1];
                    if (parts.size() > 2) {
                        _schema = parts[2];
                        if (parts.size() > 3) {
                            QString tablecol = parts[3];
                            split(tablecol, _table, _column, ".");
                            if (parts.size() > 4)
                                _rasterid = parts[4];
                        }
                    }
                }
            }
        }
        warnEmptyBasics();
    }
}

PostgresqlParameters::~PostgresqlParameters()
{
}

/**
 * @brief PostgresqlParameters::warnEmptyBasics
 * user, password, database and port can never be empty
 */

void PostgresqlParameters::warnEmptyBasics() const {
    if (_user.isEmpty())
        WARN1("Postgresql: Property '%1' is null or empty.", "Username");
    if (_pass.isEmpty())
        WARN1("Postgresql: Property '%1' is null or empty.", "Password");
    if (_host.isEmpty())
        WARN1("Postgresql: Property '%1' is null or empty.", "Host");
    if (_port == 0)
        WARN1("Postgresql: Property '%1' is null or empty.", "Port");
}

QString PostgresqlParameters::user() const
{
    return _user;
}

QString PostgresqlParameters::pass() const
{
    return _pass;
}

QString PostgresqlParameters::host() const
{
    return _host;
}

quint32 PostgresqlParameters::port() const
{
    return _port;
}

QString PostgresqlParameters::database() const
{
    return _database;
}

QString PostgresqlParameters::schema() const
{
    return _schema;
}

QString PostgresqlParameters::table() const
{
    return _table;
}

QString PostgresqlParameters::column() const
{
    return _column;
}

QString PostgresqlParameters::rasterID() const
{
    return _rasterid;
}

/*

void PostgresqlParameters::setUser(const QString &user)
{
    _user = user;
}

void PostgresqlParameters::setPass(const QString &pass)
{
    _pass = pass;
}

void PostgresqlParameters::setHost(const QString &host)
{
    _host = host;
}

void PostgresqlParameters::setPort(const quint32 &port)
{
    _port = port;
}
*/

void PostgresqlParameters::setDatabase(const QString &database)
{
    _database = database;
}

void PostgresqlParameters::setSchema(const QString &schema)
{
    _schema = schema;
}

void PostgresqlParameters::setTable(const QString &table)
{
    _table = table;
}

void PostgresqlParameters::setColumn(const QString &column)
{
    _column = column;
}

void PostgresqlParameters::setRasterID(const QString &rasterid)
{
    _rasterid = rasterid;
}

QString PostgresqlParameters::toString() const
{
    QString expression = "postgresql://" + _user + ":" + _pass + "@" + _host + ":" + QString::number(_port);
    if (!_database.isEmpty()) {
        expression += "/" + _database;
        if (!_schema.isEmpty()) {
            expression += "/" + _schema;
            if (!_table.isEmpty()) {
                if (!_column.isEmpty()) {
                    expression += "/" + merge(_table, _column, ".");
                    if (!_rasterid.isEmpty())
                        expression += "/" + _rasterid;
                } else
                    expression += "/" + _table;
            }
        }
    }
    return expression;
}

QString PostgresqlParameters::toStringBasics() const
{
    QString expression = "postgresql://" + _user + ":" + _pass + "@" + _host + ":" + QString::number(_port);
    if (!_database.isEmpty()) {
        expression += "/" + _database;
    }
    return expression;
}

bool PostgresqlParameters::valid() const
{
    return !_user.isEmpty() && !_pass.isEmpty() && !_host.isEmpty() && !(_port == 0);
}

void PostgresqlParameters::toIOOptions(IOOptions & options) const
{
    if (!_host.isEmpty())
        options.addOption("host", _host);
    if (_port != 0)
        options.addOption("port", _port);
    if (!_database.isEmpty())
        options.addOption("database", _database);
    if (!_schema.isEmpty())
        options.addOption("pg.schema", _schema);
    if (!_table.isEmpty())
        options.addOption("pg.table", _table);
    if (!_column.isEmpty())
        options.addOption("pg.column", _column);
    if (!_rasterid.isEmpty())
        options.addOption("pg.rasterid", _rasterid);
    if (!_user.isEmpty())
        options.addOption("pg.username", _user);
    if (!_pass.isEmpty())
        options.addOption("pg.password", _pass);
}

void PostgresqlParameters::replaceString(QString &str, const QString &search, const QString &replace ) const {
    for( qint32 pos = 0; ; pos += replace.length() ) {
        pos = str.indexOf(search, pos );
        if( pos < 0 )
            break;
        str = str.left(pos) + replace + str.mid(pos + search.length());
    }
}

QString PostgresqlParameters::merge(QString left, QString right, QString delimiter) const {
    replaceString(left, delimiter, "-" + delimiter);
    replaceString(right, delimiter, "-" + delimiter);
    return left + delimiter + right;
}

void PostgresqlParameters::split(QString fileName, QString & left, QString & right, QString delimiter) const {
    int iPos = fileName.indexOf(delimiter);
    while (iPos > 0 && fileName[iPos - 1] == '-')
        iPos = fileName.indexOf(delimiter, iPos + 1);
    if (iPos > 0) {
        left = fileName.left(iPos);
        right = fileName.mid(iPos + 1);
        replaceString(left, "-" + delimiter, delimiter);
        replaceString(right, "-" + delimiter, delimiter);
    } else {
        left = fileName;
        right = "";
    }
}

#ifndef SQLSTATEMENTHELPER_H
#define SQLSTATEMENTHELPER_H


#include "postgresqlparameters.h"

namespace Ilwis {

class SPFeatureI;

namespace Postgresql {

class SqlStatementHelper
{
public:
    SqlStatementHelper(const PostgresqlParameters &params);
    ~SqlStatementHelper();

    void addCreateTempTableStmt(const QString &tmpTableName);
    void addInsertChangedDataToTempTableStmt(const QString &tmpTable, const Table *table);
    void addUpdateStmt(const QList<QString> & primaryKeys, const QString &tmpTable, const Table *table);
    void addInsertStmt(const QList<QString> & primaryKeys, const QString &tmpTable, const Table *table);
    void addDeleteStmt(const QString &tmpTable, const Table *table);

    QString columnNamesCommaSeparated(const Table *table) const;
    QString columnValuesCommaSeparated(const SPFeatureI feature) const;
    QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;
    QString trimAndRemoveLastCharacter(const QString &string) const;

    QString sql();

private:
    QString createWhereComparingPrimaryKeys(const QList<QString> & primaryKeys, const QString &first, const QString &second) const;
    PostgresqlParameters _params;
    QList<QString> _tmpTables;
    QString _sqlBuilder;
};

}
}

#endif // SQLSTATEMENTHELPER_H
